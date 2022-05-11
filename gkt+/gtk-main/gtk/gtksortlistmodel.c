/*
 * Copyright © 2018 Benjamin Otte
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Benjamin Otte <otte@gnome.org>
 */

#include "config.h"

#include "gtksortlistmodel.h"

#include "gtkbitset.h"
#include "gtkintl.h"
#include "gtkprivate.h"
#include "gtksorterprivate.h"
#include "timsort/gtktimsortprivate.h"

/* The maximum amount of items to merge for a single merge step
 *
 * Making this smaller will result in more steps, which has more overhead and slows
 * down total sort time.
 * Making it larger will result in fewer steps, which increases the time taken for
 * a single step.
 *
 * As merges are the most expensive steps, this is essentially a tunable for the
 * longest time spent in gtk_tim_sort_step().
 *
 * Note that this should be reset to 0 when not doing incremental sorting to get
 * rid of all the overhead.
 */
#define GTK_SORT_MAX_MERGE_SIZE (1024)

/* Time we spend in the sort callback before returning to the main loop
 *
 * Increasing this number will make the callback take longer and potentially
 * reduce responsiveness of an application, but will increase the amount of
 * work done per step. And we emit an ::items-changed() signal after every
 * step, so if we can avoid that, we recuce the overhead in the list widget
 * and in turn reduce the total sort time.
 */
#define GTK_SORT_STEP_TIME_US (1000) /* 1 millisecond */

/**
 * GtkSortListModel:
 *
 * A `GListModel` that sorts the elements of an underlying model
 * according to a `GtkSorter`.
 *
 * The model is a stable sort. If two items compare equal according
 * to the sorter, the one that appears first in the original model will
 * also appear first after sorting.
 * Note that if you change the sorter, the previous order will have no
 * influence on the new order. If you want that, consider using a
 * `GtkMultiSorter` and appending the previous sorter to it.
 *
 * The model can be set up to do incremental sorting, so that
 * sorting long lists doesn't block the UI. See
 * [method@Gtk.SortListModel.set_incremental] for details.
 *
 * `GtkSortListModel` is a generic model and because of that it
 * cannot take advantage of any external knowledge when sorting.
 * If you run into performance issues with `GtkSortListModel`,
 * it is strongly recommended that you write your own sorting list
 * model.
 */

enum {
  PROP_0,
  PROP_INCREMENTAL,
  PROP_MODEL,
  PROP_PENDING,
  PROP_SORTER,
  NUM_PROPERTIES
};

struct _GtkSortListModel
{
  GObject parent_instance;

  GListModel *model;
  GtkSorter *sorter;
  gboolean incremental;

  GtkTimSort sort; /* ongoing sort operation */
  guint sort_cb; /* 0 or current ongoing sort callback */

  guint n_items;
  GtkSortKeys *sort_keys;
  gsize key_size;
  gpointer keys;
  GtkBitset *missing_keys;

  gpointer *positions;
};

struct _GtkSortListModelClass
{
  GObjectClass parent_class;
};

static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

static guint
pos_from_key (GtkSortListModel *self,
              gpointer          key)
{
  guint pos = ((char *) key - (char *) self->keys) / self->key_size;

  g_assert (pos < self->n_items);

  return pos;
}

static gpointer
key_from_pos (GtkSortListModel *self,
              guint             pos)
{
  return (char *) self->keys + self->key_size * pos;
}

static GType
gtk_sort_list_model_get_item_type (GListModel *list)
{
  return G_TYPE_OBJECT;
}

static guint
gtk_sort_list_model_get_n_items (GListModel *list)
{
  GtkSortListModel *self = GTK_SORT_LIST_MODEL (list);

  if (self->model == NULL)
    return 0;

  return g_list_model_get_n_items (self->model);
}

static gpointer
gtk_sort_list_model_get_item (GListModel *list,
                              guint       position)
{
  GtkSortListModel *self = GTK_SORT_LIST_MODEL (list);

  if (self->model == NULL)
    return NULL;

  if (position >= self->n_items)
    return NULL;

  if (self->positions)
    position = pos_from_key (self, self->positions[position]);

  return g_list_model_get_item (self->model, position);
}

static void
gtk_sort_list_model_model_init (GListModelInterface *iface)
{
  iface->get_item_type = gtk_sort_list_model_get_item_type;
  iface->get_n_items = gtk_sort_list_model_get_n_items;
  iface->get_item = gtk_sort_list_model_get_item;
}

G_DEFINE_TYPE_WITH_CODE (GtkSortListModel, gtk_sort_list_model, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, gtk_sort_list_model_model_init))

static gboolean
gtk_sort_list_model_is_sorting (GtkSortListModel *self)
{
  return self->sort_cb != 0;
}

static void
gtk_sort_list_model_stop_sorting (GtkSortListModel *self,
                                  gsize            *runs)
{
  if (self->sort_cb == 0)
    {
      if (runs)
        {
          runs[0] = self->n_items;
          runs[1] = 0;
        }
      return;
    }

  if (runs)
    gtk_tim_sort_get_runs (&self->sort, runs);
  gtk_tim_sort_finish (&self->sort);
  g_clear_handle_id (&self->sort_cb, g_source_remove);

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_PENDING]);
}

static gboolean
gtk_sort_list_model_sort_step (GtkSortListModel *self,
                               gboolean          finish,
                               guint            *out_position,
                               guint            *out_n_items)
{
  gint64 end_time = g_get_monotonic_time ();
  gboolean result = FALSE;
  GtkTimSortRun change;
  gpointer *start_change, *end_change;

  end_time += GTK_SORT_STEP_TIME_US;

  if (!gtk_bitset_is_empty (self->missing_keys))
    {
      GtkBitsetIter iter;
      guint pos;

      for (gtk_bitset_iter_init_first (&iter, self->missing_keys, &pos);
           gtk_bitset_iter_is_valid (&iter);
           gtk_bitset_iter_next (&iter, &pos))
        {
          gpointer item = g_list_model_get_item (self->model, pos);
          gtk_sort_keys_init_key (self->sort_keys, item, key_from_pos (self, pos));
          g_object_unref (item);

          if (g_get_monotonic_time () >= end_time && !finish)
            {
              gtk_bitset_remove_range_closed (self->missing_keys, 0, pos);
              *out_position = 0;
              *out_n_items = 0;
              return TRUE;
            }
        }
      result = TRUE;
      gtk_bitset_remove_all (self->missing_keys);
    }

  end_change = self->positions;
  start_change = self->positions + self->n_items;

  while (gtk_tim_sort_step (&self->sort, &change))
    {
      result = TRUE;
      if (change.len)
        {
          start_change = MIN (start_change, (gpointer *) change.base);
          end_change = MAX (end_change, ((gpointer *) change.base) + change.len);
        }
     
      if (g_get_monotonic_time () >= end_time && !finish)
        break;
    }

  if (start_change < end_change)
    {
      *out_position = start_change - self->positions;
      *out_n_items = end_change - start_change;
    }
  else
    {
      *out_position = 0;
      *out_n_items = 0;
    }

  return result;
}

static gboolean
gtk_sort_list_model_sort_cb (gpointer data)
{
  GtkSortListModel *self = data;
  guint pos, n_items;

  if (gtk_sort_list_model_sort_step (self, FALSE, &pos, &n_items))
    {
      if (n_items)
        g_list_model_items_changed (G_LIST_MODEL (self), pos, n_items, n_items);
      g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_PENDING]);
      return G_SOURCE_CONTINUE;
    }

  gtk_sort_list_model_stop_sorting (self, NULL);
  return G_SOURCE_REMOVE;
}

static int
sort_func (gconstpointer a,
           gconstpointer b,
           gpointer      data)
{
  gpointer *sa = (gpointer *) a;
  gpointer *sb = (gpointer *) b;
  int result;

  result = gtk_sort_keys_compare (data, *sa, *sb);
  if (result)
    return result;

  return *sa < *sb ? -1 : 1;
}

static gboolean
gtk_sort_list_model_start_sorting (GtkSortListModel *self,
                                   gsize            *runs)
{
  g_assert (self->sort_cb == 0);

  gtk_tim_sort_init (&self->sort,
                     self->positions,
                     self->n_items,
                     sizeof (gpointer),
                     sort_func,
                     self->sort_keys);
  if (runs)
    gtk_tim_sort_set_runs (&self->sort, runs);
  if (self->incremental)
    gtk_tim_sort_set_max_merge_size (&self->sort, GTK_SORT_MAX_MERGE_SIZE);

  if (!self->incremental)
    return FALSE;

  self->sort_cb = g_idle_add (gtk_sort_list_model_sort_cb, self);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_PENDING]);
  return TRUE;
}

static void
gtk_sort_list_model_finish_sorting (GtkSortListModel *self,
                                    guint            *pos,
                                    guint            *n_items)
{
  gtk_tim_sort_set_max_merge_size (&self->sort, 0);

  gtk_sort_list_model_sort_step (self, TRUE, pos, n_items);
  gtk_tim_sort_finish (&self->sort);

  gtk_sort_list_model_stop_sorting (self, NULL);
}

static void
gtk_sort_list_model_clear_sort_keys (GtkSortListModel *self,
                                     guint             position,
                                     guint             n_items)
{
  GtkBitsetIter iter;
  GtkBitset *clear;
  guint pos;

  if (!gtk_sort_keys_needs_clear_key (self->sort_keys))
    return;

  clear = gtk_bitset_new_range (position, n_items);
  gtk_bitset_subtract (clear, self->missing_keys);

  for (gtk_bitset_iter_init_first (&iter, clear, &pos);
       gtk_bitset_iter_is_valid (&iter);
       gtk_bitset_iter_next (&iter, &pos))
    {
      gtk_sort_keys_clear_key (self->sort_keys, key_from_pos (self, pos));
    }

  gtk_bitset_unref (clear);
}

static void
gtk_sort_list_model_clear_keys (GtkSortListModel *self)
{
  gtk_sort_list_model_clear_sort_keys (self, 0, self->n_items);

  g_clear_pointer (&self->missing_keys, gtk_bitset_unref);
  g_clear_pointer (&self->keys, g_free);
  g_clear_pointer (&self->sort_keys, gtk_sort_keys_unref);
  self->key_size = 0;
}

static void
gtk_sort_list_model_clear_items (GtkSortListModel *self,
                                 guint            *pos,
                                 guint            *n_items)
{
  gtk_sort_list_model_stop_sorting (self, NULL);

  if (self->sort_keys == NULL)
    {
      if (pos || n_items)
        *pos = *n_items = 0;
      return;
    }

  if (pos || n_items)
    {
      guint start, end;

      for (start = 0; start < self->n_items; start++)
        {
          if (pos_from_key (self, self->positions[start]) != + start)
            break;
        }
      for (end = self->n_items; end > start; end--)
        {
          if (pos_from_key (self, self->positions[end - 1]) != end - 1)
            break;
        }

      *n_items = end - start;
      if (*n_items == 0)
        *pos = 0;
      else
        *pos = start;
    }

  g_clear_pointer (&self->positions, g_free);

  gtk_sort_list_model_clear_keys (self);
} 

static gboolean
gtk_sort_list_model_should_sort (GtkSortListModel *self)
{
  return self->sorter != NULL &&
         self->model != NULL &&
         gtk_sorter_get_order (self->sorter) != GTK_SORTER_ORDER_NONE;
}

static void
gtk_sort_list_model_create_keys (GtkSortListModel *self)
{
  g_assert (self->keys == NULL);
  g_assert (self->sort_keys == NULL);
  g_assert (self->key_size == 0);

  self->sort_keys = gtk_sorter_get_keys (self->sorter);
  self->key_size = gtk_sort_keys_get_key_size (self->sort_keys);
  self->keys = g_malloc_n (self->n_items, self->key_size);
  self->missing_keys = gtk_bitset_new_range (0, self->n_items);
}

static void
gtk_sort_list_model_create_items (GtkSortListModel *self)
{
  guint i;

  if (!gtk_sort_list_model_should_sort (self))
    return;

  g_assert (self->sort_keys == NULL);

  self->positions = g_new (gpointer, self->n_items);

  gtk_sort_list_model_create_keys (self);

  for (i = 0; i < self->n_items; i++)
    self->positions[i] = key_from_pos (self, i);
}

/* This realloc()s the arrays but does not set the added values. */
static void
gtk_sort_list_model_update_items (GtkSortListModel *self,
                                  gsize             runs[GTK_TIM_SORT_MAX_PENDING + 1],
                                  guint             position,
                                  guint             removed,
                                  guint             added,
                                  guint            *unmodified_start,
                                  guint            *unmodified_end)
{
  guint i, n_items, valid;
  guint run_index, valid_run, valid_run_end, run_end;
  guint start, end;
  gpointer *old_keys;

  n_items = self->n_items;
  start = n_items;
  end = n_items;
  
  /* first, move the keys over */
  old_keys = self->keys;
  gtk_sort_list_model_clear_sort_keys (self, position, removed);

  if (removed > added)
    {
      memmove (key_from_pos (self, position + added),
               key_from_pos (self, position + removed),
               self->key_size * (n_items - position - removed));
      self->keys = g_realloc_n (self->keys, n_items - removed + added, self->key_size);
    }
  else if (removed < added)
    {
      self->keys = g_realloc_n (self->keys, n_items - removed + added, self->key_size);
      memmove (key_from_pos (self, position + added),
               key_from_pos (self, position + removed),
               self->key_size * (n_items - position - removed));
    }

  /* then, update the positions */
  valid = 0;
  valid_run = 0;
  valid_run_end = 0;
  run_index = 0;
  run_end = 0;
  for (i = 0; i < n_items;)
    {
      if (runs[run_index] == 0)
        {
          run_end = n_items;
          valid_run_end = G_MAXUINT;
        }
      else
        {
          run_end += runs[run_index++];
        }

      for (; i < run_end; i++)
        {
          guint pos = ((char *) self->positions[i] - (char *) old_keys) / self->key_size;

          if (pos >= position + removed)
            pos = pos - removed + added;
          else if (pos >= position)
            { 
              start = MIN (start, valid);
              end = n_items - i - 1;
              continue;
            }

          self->positions[valid] = key_from_pos (self, pos);
          valid++;
        }

      if (valid_run_end < valid)
        {
          runs[valid_run++] = valid - valid_run_end;
          valid_run_end = valid;
        }
    }
  g_assert (i == n_items);
  g_assert (valid == n_items - removed);
  runs[valid_run] = 0;

  self->positions = g_renew (gpointer, self->positions, n_items - removed + added);

  if (self->missing_keys)
    {
      gtk_bitset_splice (self->missing_keys, position, removed, added);
      gtk_bitset_add_range (self->missing_keys, position, added);
    }

  self->n_items = n_items - removed + added;

  for (i = 0; i < added; i++)
    {
      self->positions[self->n_items - added + i] = key_from_pos (self, position + i);
    }

  *unmodified_start = start;
  *unmodified_end = end;
}

static void
gtk_sort_list_model_items_changed_cb (GListModel       *model,
                                      guint             position,
                                      guint             removed,
                                      guint             added,
                                      GtkSortListModel *self)
{
  gsize runs[GTK_TIM_SORT_MAX_PENDING + 1];
  guint i, n_items, start, end;
  gboolean was_sorting;

  if (removed == 0 && added == 0)
    return;

  if (!gtk_sort_list_model_should_sort (self))
    {
      self->n_items = self->n_items - removed + added;
      g_list_model_items_changed (G_LIST_MODEL (self), position, removed, added);
      return;
    }

  was_sorting = gtk_sort_list_model_is_sorting (self);
  gtk_sort_list_model_stop_sorting (self, runs);

  gtk_sort_list_model_update_items (self, runs, position, removed, added, &start, &end);

  if (added > 0)
    {
      if (gtk_sort_list_model_start_sorting (self, runs))
        {
          end = 0;
        }
      else
        {
          guint pos, n;
          gtk_sort_list_model_finish_sorting (self, &pos, &n);
          if (n)
            start = MIN (start, pos);
          /* find first item that was added */
          for (i = 0; i < end; i++)
            {
              pos = pos_from_key (self, self->positions[self->n_items - i - 1]);
              if (pos >= position && pos < position + added)
                {
                  end = i;
                  break;
                }
            }
        }
    }
  else
    {
      if (was_sorting)
        gtk_sort_list_model_start_sorting (self, runs);
    }

  n_items = self->n_items - start - end;
  g_list_model_items_changed (G_LIST_MODEL (self), start, n_items - added + removed, n_items);
}

static void
gtk_sort_list_model_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  GtkSortListModel *self = GTK_SORT_LIST_MODEL (object);

  switch (prop_id)
    {
    case PROP_INCREMENTAL:
      gtk_sort_list_model_set_incremental (self, g_value_get_boolean (value));
      break;

    case PROP_MODEL:
      gtk_sort_list_model_set_model (self, g_value_get_object (value));
      break;

    case PROP_SORTER:
      gtk_sort_list_model_set_sorter (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void 
gtk_sort_list_model_get_property (GObject     *object,
                                  guint        prop_id,
                                  GValue      *value,
                                  GParamSpec  *pspec)
{
  GtkSortListModel *self = GTK_SORT_LIST_MODEL (object);

  switch (prop_id)
    {
    case PROP_INCREMENTAL:
      g_value_set_boolean (value, self->incremental);
      break;

    case PROP_MODEL:
      g_value_set_object (value, self->model);
      break;

    case PROP_PENDING:
      g_value_set_uint (value, gtk_sort_list_model_get_pending (self));
      break;

    case PROP_SORTER:
      g_value_set_object (value, self->sorter);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_sort_list_model_sorter_changed_cb (GtkSorter        *sorter,
                                       int               change,
                                       GtkSortListModel *self)
{
  guint pos, n_items;

  if (gtk_sort_list_model_should_sort (self))
    {
      gtk_sort_list_model_stop_sorting (self, NULL);

      if (self->sort_keys == NULL)
        {
          gtk_sort_list_model_create_items (self);
        }
      else
        {
          GtkSortKeys *new_keys = gtk_sorter_get_keys (sorter);

          if (!gtk_sort_keys_is_compatible (new_keys, self->sort_keys))
            {
              char *old_keys = self->keys;
              gsize old_key_size = self->key_size;
              guint i;

              gtk_sort_list_model_clear_keys (self);
              gtk_sort_list_model_create_keys (self);

              for (i = 0; i < self->n_items; i++)
                self->positions[i] = key_from_pos (self, ((char *) self->positions[i] - old_keys) / old_key_size);

              gtk_sort_keys_unref (new_keys);
            }
          else
            {
              gtk_sort_keys_unref (self->sort_keys);
              self->sort_keys = new_keys;
            }
        }

      if (gtk_sort_list_model_start_sorting (self, NULL))
        pos = n_items = 0;
      else
        gtk_sort_list_model_finish_sorting (self, &pos, &n_items);
    }
  else
    {
      gtk_sort_list_model_clear_items (self, &pos, &n_items);
    }

  if (n_items > 0)
    g_list_model_items_changed (G_LIST_MODEL (self), pos, n_items, n_items);
}

static void
gtk_sort_list_model_clear_model (GtkSortListModel *self)
{
  if (self->model == NULL)
    return;

  g_signal_handlers_disconnect_by_func (self->model, gtk_sort_list_model_items_changed_cb, self);
  g_clear_object (&self->model);
  gtk_sort_list_model_clear_items (self, NULL, NULL);
  self->n_items = 0;
}

static void
gtk_sort_list_model_clear_sorter (GtkSortListModel *self)
{
  if (self->sorter == NULL)
    return;

  g_signal_handlers_disconnect_by_func (self->sorter, gtk_sort_list_model_sorter_changed_cb, self);
  g_clear_object (&self->sorter);
}

static void
gtk_sort_list_model_dispose (GObject *object)
{
  GtkSortListModel *self = GTK_SORT_LIST_MODEL (object);

  gtk_sort_list_model_clear_model (self);
  gtk_sort_list_model_clear_sorter (self);

  G_OBJECT_CLASS (gtk_sort_list_model_parent_class)->dispose (object);
};

static void
gtk_sort_list_model_class_init (GtkSortListModelClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);

  gobject_class->set_property = gtk_sort_list_model_set_property;
  gobject_class->get_property = gtk_sort_list_model_get_property;
  gobject_class->dispose = gtk_sort_list_model_dispose;

  /**
   * GtkSortListModel:incremental: (attributes org.gtk.Property.get=gtk_sort_list_model_get_incremental org.gtk.Property.set=gtk_sort_list_model_set_incremental)
   *
   * If the model should sort items incrementally.
   */
  properties[PROP_INCREMENTAL] =
      g_param_spec_boolean ("incremental",
                            P_("Incremental"),
                            P_("Sort items incrementally"),
                            FALSE,
                            GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkSortListModel:model: (attributes org.gtk.Property.get=gtk_sort_list_model_get_model org.gtk.Property.set=gtk_sort_list_model_set_model)
   *
   * The model being sorted.
   */
  properties[PROP_MODEL] =
      g_param_spec_object ("model",
                           P_("Model"),
                           P_("The model being sorted"),
                           G_TYPE_LIST_MODEL,
                           GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkSortListModel:pending: (attributes org.gtk.Property.get=gtk_sort_list_model_get_pending)
   *
   * Estimate of unsorted items remaining.
   */
  properties[PROP_PENDING] =
      g_param_spec_uint ("pending",
                         P_("Pending"),
                         P_("Estimate of unsorted items remaining"),
                         0, G_MAXUINT, 0,
                         GTK_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkSortListModel:sorter: (attributes org.gtk.Property.get=gtk_sort_list_model_get_sorter org.gtk.Property.set=gtk_sort_list_model_set_sorter)
   *
   * The sorter for this model.
   */
  properties[PROP_SORTER] =
      g_param_spec_object ("sorter",
                            P_("Sorter"),
                            P_("The sorter for this model"),
                            GTK_TYPE_SORTER,
                            GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (gobject_class, NUM_PROPERTIES, properties);
}

static void
gtk_sort_list_model_init (GtkSortListModel *self)
{
}

/**
 * gtk_sort_list_model_new:
 * @model: (nullable) (transfer full): the model to sort
 * @sorter: (nullable) (transfer full): the `GtkSorter` to sort @model with,
 *
 * Creates a new sort list model that uses the @sorter to sort @model.
 *
 * Returns: a new `GtkSortListModel`
 */
GtkSortListModel *
gtk_sort_list_model_new (GListModel *model,
                         GtkSorter  *sorter)
{
  GtkSortListModel *result;

  g_return_val_if_fail (model == NULL || G_IS_LIST_MODEL (model), NULL);
  g_return_val_if_fail (sorter == NULL || GTK_IS_SORTER (sorter), NULL);

  result = g_object_new (GTK_TYPE_SORT_LIST_MODEL,
                         "model", model,
                         "sorter", sorter,
                         NULL);

  /* consume the references */
  g_clear_object (&model);
  g_clear_object (&sorter);

  return result;
}

/**
 * gtk_sort_list_model_set_model: (attributes org.gtk.Method.set_property=model)
 * @self: a `GtkSortListModel`
 * @model: (nullable): The model to be sorted
 *
 * Sets the model to be sorted.
 *
 * The @model's item type must conform to the item type of @self.
 */
void
gtk_sort_list_model_set_model (GtkSortListModel *self,
                               GListModel       *model)
{
  guint removed;

  g_return_if_fail (GTK_IS_SORT_LIST_MODEL (self));
  g_return_if_fail (model == NULL || G_IS_LIST_MODEL (model));

  if (self->model == model)
    return;

  removed = g_list_model_get_n_items (G_LIST_MODEL (self));
  gtk_sort_list_model_clear_model (self);

  if (model)
    {
      guint ignore1, ignore2;

      self->model = g_object_ref (model);
      self->n_items = g_list_model_get_n_items (model);
      g_signal_connect (model, "items-changed", G_CALLBACK (gtk_sort_list_model_items_changed_cb), self);

      if (gtk_sort_list_model_should_sort (self))
        {
          gtk_sort_list_model_create_items (self);
          if (!gtk_sort_list_model_start_sorting (self, NULL))
            gtk_sort_list_model_finish_sorting (self, &ignore1, &ignore2);
        }
    }
  
  if (removed > 0 || self->n_items > 0)
    g_list_model_items_changed (G_LIST_MODEL (self), 0, removed, self->n_items);

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_MODEL]);
}

/**
 * gtk_sort_list_model_get_model: (attributes org.gtk.Method.get_property=model)
 * @self: a `GtkSortListModel`
 *
 * Gets the model currently sorted or %NULL if none.
 *
 * Returns: (nullable) (transfer none): The model that gets sorted
 */
GListModel *
gtk_sort_list_model_get_model (GtkSortListModel *self)
{
  g_return_val_if_fail (GTK_IS_SORT_LIST_MODEL (self), NULL);

  return self->model;
}

/**
 * gtk_sort_list_model_set_sorter: (attributes org.gtk.Method.set_property=sorter)
 * @self: a `GtkSortListModel`
 * @sorter: (nullable): the `GtkSorter` to sort @model with
 *
 * Sets a new sorter on @self.
 */
void
gtk_sort_list_model_set_sorter (GtkSortListModel *self,
                                GtkSorter        *sorter)
{
  g_return_if_fail (GTK_IS_SORT_LIST_MODEL (self));
  g_return_if_fail (sorter == NULL || GTK_IS_SORTER (sorter));

  gtk_sort_list_model_clear_sorter (self);

  if (sorter)
    {
      self->sorter = g_object_ref (sorter);
      g_signal_connect (sorter, "changed", G_CALLBACK (gtk_sort_list_model_sorter_changed_cb), self);
    }

  gtk_sort_list_model_sorter_changed_cb (sorter, GTK_SORTER_CHANGE_DIFFERENT, self);

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_SORTER]);
}

/**
 * gtk_sort_list_model_get_sorter: (attributes org.gtk.Method.get_property=sorter)
 * @self: a `GtkSortListModel`
 *
 * Gets the sorter that is used to sort @self.
 *
 * Returns: (nullable) (transfer none): the sorter of #self
 */
GtkSorter *
gtk_sort_list_model_get_sorter (GtkSortListModel *self)
{
  g_return_val_if_fail (GTK_IS_SORT_LIST_MODEL (self), NULL);

  return self->sorter;
}

/**
 * gtk_sort_list_model_set_incremental: (attributes org.gtk.Method.set_property=incremental)
 * @self: a `GtkSortListModel`
 * @incremental: %TRUE to sort incrementally
 *
 * Sets the sort model to do an incremental sort.
 *
 * When incremental sorting is enabled, the `GtkSortListModel` will not do
 * a complete sort immediately, but will instead queue an idle handler that
 * incrementally sorts the items towards their correct position. This of
 * course means that items do not instantly appear in the right place. It
 * also means that the total sorting time is a lot slower.
 *
 * When your filter blocks the UI while sorting, you might consider
 * turning this on. Depending on your model and sorters, this may become
 * interesting around 10,000 to 100,000 items.
 *
 * By default, incremental sorting is disabled.
 *
 * See [method@Gtk.SortListModel.get_pending] for progress information
 * about an ongoing incremental sorting operation.
 */
void
gtk_sort_list_model_set_incremental (GtkSortListModel *self,
                                     gboolean          incremental)
{
  g_return_if_fail (GTK_IS_SORT_LIST_MODEL (self));

  if (self->incremental == incremental)
    return;

  self->incremental = incremental;

  if (!incremental && gtk_sort_list_model_is_sorting (self))
    {
      guint pos, n_items;

      gtk_sort_list_model_finish_sorting (self, &pos, &n_items);
      if (n_items)
        g_list_model_items_changed (G_LIST_MODEL (self), pos, n_items, n_items);
    }

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_INCREMENTAL]);
}

/**
 * gtk_sort_list_model_get_incremental: (attributes org.gtk.Method.get_property=incremental)
 * @self: a `GtkSortListModel`
 *
 * Returns whether incremental sorting is enabled.
 *
 * See [method@Gtk.SortListModel.set_incremental].
 *
 * Returns: %TRUE if incremental sorting is enabled
 */
gboolean
gtk_sort_list_model_get_incremental (GtkSortListModel *self)
{
  g_return_val_if_fail (GTK_IS_SORT_LIST_MODEL (self), FALSE);

  return self->incremental;
}

/**
 * gtk_sort_list_model_get_pending: (attributes org.gtk.Method.get_property=pending)
 * @self: a `GtkSortListModel`
 *
 * Estimates progress of an ongoing sorting operation.
 *
 * The estimate is the number of items that would still need to be
 * sorted to finish the sorting operation if this was a linear
 * algorithm. So this number is not related to how many items are
 * already correctly sorted.
 *
 * If you want to estimate the progress, you can use code like this:
 * ```c
 * pending = gtk_sort_list_model_get_pending (self);
 * model = gtk_sort_list_model_get_model (self);
 * progress = 1.0 - pending / (double) MAX (1, g_list_model_get_n_items (model));
 * ```
 *
 * If no sort operation is ongoing - in particular when
 * [property@Gtk.SortListModel:incremental] is %FALSE - this
 * function returns 0.
 *
 * Returns: a progress estimate of remaining items to sort
 */
guint
gtk_sort_list_model_get_pending (GtkSortListModel *self)
{
  g_return_val_if_fail (GTK_IS_SORT_LIST_MODEL (self), FALSE);

  if (self->sort_cb == 0)
    return 0;

  /* We do a random guess that 50% of time is spent generating keys
   * and the other 50% is spent actually sorting.
   *
   * This is of course massively wrong, but it depends on the sorter
   * in use, and estimating this correctly is hard, so this will have
   * to be good enough.
   */
  if (!gtk_bitset_is_empty (self->missing_keys))
    {
      return (self->n_items + gtk_bitset_get_size (self->missing_keys)) / 2;
    }
  else
    {
      return (self->n_items - gtk_tim_sort_get_progress (&self->sort)) / 2;
    }
}

