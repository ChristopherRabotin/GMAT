/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * GNode: N-way tree implementation.
 * Copyright (C) 1998 Tim Janik
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */

/* 
 * MT safe
 */

#include "config.h"

#include "glib.h"
#include "galias.h"

void g_node_push_allocator (gpointer dummy) { /* present for binary compat only */ }
void g_node_pop_allocator  (void)           { /* present for binary compat only */ }

#define g_node_alloc0()         g_slice_new0 (GNode)
#define g_node_free(node)       g_slice_free (GNode, node)

/* --- functions --- */
GNode*
g_node_new (gpointer data)
{
  GNode *node = g_node_alloc0();
  node->data = data;
  return node;
}

static void
g_nodes_free (GNode *node)
{
  while (node)
    {
      GNode *next = node->next;
      if (node->children)
        g_nodes_free (node->children);
      g_node_free (node);
      node = next;
    }
}

void
g_node_destroy (GNode *root)
{
  g_return_if_fail (root != NULL);
  
  if (!G_NODE_IS_ROOT (root))
    g_node_unlink (root);
  
  g_nodes_free (root);
}

void
g_node_unlink (GNode *node)
{
  g_return_if_fail (node != NULL);
  
  if (node->prev)
    node->prev->next = node->next;
  else if (node->parent)
    node->parent->children = node->next;
  node->parent = NULL;
  if (node->next)
    {
      node->next->prev = node->prev;
      node->next = NULL;
    }
  node->prev = NULL;
}

/**
 * g_node_copy_deep:
 * @node: a #GNode
 * @copy_func: the function which is called to copy the data inside each node,
 *   or %NULL to use the original data.
 * @data: data to pass to @copy_func
 * 
 * Recursively copies a #GNode and its data.
 * 
 * Return value: a new #GNode containing copies of the data in @node.
 *
 * Since: 2.4
 **/
GNode*
g_node_copy_deep (GNode     *node, 
		  GCopyFunc  copy_func,
		  gpointer   data)
{
  GNode *new_node = NULL;

  if (copy_func == NULL)
	return g_node_copy (node);

  if (node)
    {
      GNode *child, *new_child;
      
      new_node = g_node_new (copy_func (node->data, data));
      
      for (child = g_node_last_child (node); child; child = child->prev) 
	{
	  new_child = g_node_copy_deep (child, copy_func, data);
	  g_node_prepend (new_node, new_child);
	}
    }
  
  return new_node;
}

GNode*
g_node_copy (GNode *node)
{
  GNode *new_node = NULL;
  
  if (node)
    {
      GNode *child;
      
      new_node = g_node_new (node->data);
      
      for (child = g_node_last_child (node); child; child = child->prev)
	g_node_prepend (new_node, g_node_copy (child));
    }
  
  return new_node;
}

GNode*
g_node_insert (GNode *parent,
	       gint   position,
	       GNode *node)
{
  g_return_val_if_fail (parent != NULL, node);
  g_return_val_if_fail (node != NULL, node);
  g_return_val_if_fail (G_NODE_IS_ROOT (node), node);
  
  if (position > 0)
    return g_node_insert_before (parent,
				 g_node_nth_child (parent, position),
				 node);
  else if (position == 0)
    return g_node_prepend (parent, node);
  else /* if (position < 0) */
    return g_node_append (parent, node);
}

GNode*
g_node_insert_before (GNode *parent,
		      GNode *sibling,
		      GNode *node)
{
  g_return_val_if_fail (parent != NULL, node);
  g_return_val_if_fail (node != NULL, node);
  g_return_val_if_fail (G_NODE_IS_ROOT (node), node);
  if (sibling)
    g_return_val_if_fail (sibling->parent == parent, node);
  
  node->parent = parent;
  
  if (sibling)
    {
      if (sibling->prev)
	{
	  node->prev = sibling->prev;
	  node->prev->next = node;
	  node->next = sibling;
	  sibling->prev = node;
	}
      else
	{
	  node->parent->children = node;
	  node->next = sibling;
	  sibling->prev = node;
	}
    }
  else
    {
      if (parent->children)
	{
	  sibling = parent->children;
	  while (sibling->next)
	    sibling = sibling->next;
	  node->prev = sibling;
	  sibling->next = node;
	}
      else
	node->parent->children = node;
    }

  return node;
}

GNode*
g_node_insert_after (GNode *parent,
		     GNode *sibling,
		     GNode *node)
{
  g_return_val_if_fail (parent != NULL, node);
  g_return_val_if_fail (node != NULL, node);
  g_return_val_if_fail (G_NODE_IS_ROOT (node), node);
  if (sibling)
    g_return_val_if_fail (sibling->parent == parent, node);

  node->parent = parent;

  if (sibling)
    {
      if (sibling->next)
	{
	  sibling->next->prev = node;
	}
      node->next = sibling->next;
      node->prev = sibling;
      sibling->next = node;
    }
  else
    {
      if (parent->children)
	{
	  node->next = parent->children;
	  parent->children->prev = node;
	}
      parent->children = node;
    }

  return node;
}

GNode*
g_node_prepend (GNode *parent,
		GNode *node)
{
  g_return_val_if_fail (parent != NULL, node);
  
  return g_node_insert_before (parent, parent->children, node);
}

GNode*
g_node_get_root (GNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);
  
  while (node->parent)
    node = node->parent;
  
  return node;
}

gboolean
g_node_is_ancestor (GNode *node,
		    GNode *descendant)
{
  g_return_val_if_fail (node != NULL, FALSE);
  g_return_val_if_fail (descendant != NULL, FALSE);
  
  while (descendant)
    {
      if (descendant->parent == node)
	return TRUE;
      
      descendant = descendant->parent;
    }
  
  return FALSE;
}

/* returns 1 for root, 2 for first level children,
 * 3 for children's children...
 */
guint
g_node_depth (GNode *node)
{
  register guint depth = 0;
  
  while (node)
    {
      depth++;
      node = node->parent;
    }
  
  return depth;
}

void
g_node_reverse_children (GNode *node)
{
  GNode *child;
  GNode *last;
  
  g_return_if_fail (node != NULL);
  
  child = node->children;
  last = NULL;
  while (child)
    {
      last = child;
      child = last->next;
      last->next = last->prev;
      last->prev = child;
    }
  node->children = last;
}

guint
g_node_max_height (GNode *root)
{
  register GNode *child;
  register guint max_height = 0;
  
  if (!root)
    return 0;
  
  child = root->children;
  while (child)
    {
      register guint tmp_height;
      
      tmp_height = g_node_max_height (child);
      if (tmp_height > max_height)
	max_height = tmp_height;
      child = child->next;
    }
  
  return max_height + 1;
}

static gboolean
g_node_traverse_pre_order (GNode	    *node,
			   GTraverseFlags    flags,
			   GNodeTraverseFunc func,
			   gpointer	     data)
{
  if (node->children)
    {
      GNode *child;
      
      if ((flags & G_TRAVERSE_NON_LEAFS) &&
	  func (node, data))
	return TRUE;
      
      child = node->children;
      while (child)
	{
	  register GNode *current;
	  
	  current = child;
	  child = current->next;
	  if (g_node_traverse_pre_order (current, flags, func, data))
	    return TRUE;
	}
    }
  else if ((flags & G_TRAVERSE_LEAFS) &&
	   func (node, data))
    return TRUE;
  
  return FALSE;
}

static gboolean
g_node_depth_traverse_pre_order (GNode		  *node,
				 GTraverseFlags	   flags,
				 guint		   depth,
				 GNodeTraverseFunc func,
				 gpointer	   data)
{
  if (node->children)
    {
      GNode *child;
      
      if ((flags & G_TRAVERSE_NON_LEAFS) &&
	  func (node, data))
	return TRUE;
      
      depth--;
      if (!depth)
	return FALSE;
      
      child = node->children;
      while (child)
	{
	  register GNode *current;
	  
	  current = child;
	  child = current->next;
	  if (g_node_depth_traverse_pre_order (current, flags, depth, func, data))
	    return TRUE;
	}
    }
  else if ((flags & G_TRAVERSE_LEAFS) &&
	   func (node, data))
    return TRUE;
  
  return FALSE;
}

static gboolean
g_node_traverse_post_order (GNode	     *node,
			    GTraverseFlags    flags,
			    GNodeTraverseFunc func,
			    gpointer	      data)
{
  if (node->children)
    {
      GNode *child;
      
      child = node->children;
      while (child)
	{
	  register GNode *current;
	  
	  current = child;
	  child = current->next;
	  if (g_node_traverse_post_order (current, flags, func, data))
	    return TRUE;
	}
      
      if ((flags & G_TRAVERSE_NON_LEAFS) &&
	  func (node, data))
	return TRUE;
      
    }
  else if ((flags & G_TRAVERSE_LEAFS) &&
	   func (node, data))
    return TRUE;
  
  return FALSE;
}

static gboolean
g_node_depth_traverse_post_order (GNode		   *node,
				  GTraverseFlags    flags,
				  guint		    depth,
				  GNodeTraverseFunc func,
				  gpointer	    data)
{
  if (node->children)
    {
      depth--;
      if (depth)
	{
	  GNode *child;
	  
	  child = node->children;
	  while (child)
	    {
	      register GNode *current;
	      
	      current = child;
	      child = current->next;
	      if (g_node_depth_traverse_post_order (current, flags, depth, func, data))
		return TRUE;
	    }
	}
      
      if ((flags & G_TRAVERSE_NON_LEAFS) &&
	  func (node, data))
	return TRUE;
      
    }
  else if ((flags & G_TRAVERSE_LEAFS) &&
	   func (node, data))
    return TRUE;
  
  return FALSE;
}

static gboolean
g_node_traverse_in_order (GNode		   *node,
			  GTraverseFlags    flags,
			  GNodeTraverseFunc func,
			  gpointer	    data)
{
  if (node->children)
    {
      GNode *child;
      register GNode *current;
      
      child = node->children;
      current = child;
      child = current->next;
      
      if (g_node_traverse_in_order (current, flags, func, data))
	return TRUE;
      
      if ((flags & G_TRAVERSE_NON_LEAFS) &&
	  func (node, data))
	return TRUE;
      
      while (child)
	{
	  current = child;
	  child = current->next;
	  if (g_node_traverse_in_order (current, flags, func, data))
	    return TRUE;
	}
    }
  else if ((flags & G_TRAVERSE_LEAFS) &&
	   func (node, data))
    return TRUE;
  
  return FALSE;
}

static gboolean
g_node_depth_traverse_in_order (GNode		 *node,
				GTraverseFlags	  flags,
				guint		  depth,
				GNodeTraverseFunc func,
				gpointer	  data)
{
  if (node->children)
    {
      depth--;
      if (depth)
	{
	  GNode *child;
	  register GNode *current;
	  
	  child = node->children;
	  current = child;
	  child = current->next;
	  
	  if (g_node_depth_traverse_in_order (current, flags, depth, func, data))
	    return TRUE;
	  
	  if ((flags & G_TRAVERSE_NON_LEAFS) &&
	      func (node, data))
	    return TRUE;
	  
	  while (child)
	    {
	      current = child;
	      child = current->next;
	      if (g_node_depth_traverse_in_order (current, flags, depth, func, data))
		return TRUE;
	    }
	}
      else if ((flags & G_TRAVERSE_NON_LEAFS) &&
	       func (node, data))
	return TRUE;
    }
  else if ((flags & G_TRAVERSE_LEAFS) &&
	   func (node, data))
    return TRUE;
  
  return FALSE;
}

static gboolean
g_node_traverse_level (GNode		 *node,
		       GTraverseFlags	  flags,
		       guint		  level,
		       GNodeTraverseFunc func,
		       gpointer	  data,
		       gboolean         *more_levels)
{
  if (level == 0) 
    {
      if (node->children)
	{
	  *more_levels = TRUE;
	  return (flags & G_TRAVERSE_NON_LEAFS) && func (node, data);
	}
      else
	{
	  return (flags & G_TRAVERSE_LEAFS) && func (node, data);
	}
    }
  else 
    {
      node = node->children;
      
      while (node)
	{
	  if (g_node_traverse_level (node, flags, level - 1, func, data, more_levels))
	    return TRUE;

	  node = node->next;
	}
    }

  return FALSE;
}

static gboolean
g_node_depth_traverse_level (GNode		 *node,
			     GTraverseFlags	  flags,
			     guint		  depth,
			     GNodeTraverseFunc func,
			     gpointer	  data)
{
  guint level;
  gboolean more_levels;

  level = 0;  
  while (level != depth) 
    {
      more_levels = FALSE;
      if (g_node_traverse_level (node, flags, level, func, data, &more_levels))
	return TRUE;
      if (!more_levels)
	break;
      level++;
    }
  return FALSE;
}

void
g_node_traverse (GNode		  *root,
		 GTraverseType	   order,
		 GTraverseFlags	   flags,
		 gint		   depth,
		 GNodeTraverseFunc func,
		 gpointer	   data)
{
  g_return_if_fail (root != NULL);
  g_return_if_fail (func != NULL);
  g_return_if_fail (order <= G_LEVEL_ORDER);
  g_return_if_fail (flags <= G_TRAVERSE_MASK);
  g_return_if_fail (depth == -1 || depth > 0);
  
  switch (order)
    {
    case G_PRE_ORDER:
      if (depth < 0)
	g_node_traverse_pre_order (root, flags, func, data);
      else
	g_node_depth_traverse_pre_order (root, flags, depth, func, data);
      break;
    case G_POST_ORDER:
      if (depth < 0)
	g_node_traverse_post_order (root, flags, func, data);
      else
	g_node_depth_traverse_post_order (root, flags, depth, func, data);
      break;
    case G_IN_ORDER:
      if (depth < 0)
	g_node_traverse_in_order (root, flags, func, data);
      else
	g_node_depth_traverse_in_order (root, flags, depth, func, data);
      break;
    case G_LEVEL_ORDER:
      g_node_depth_traverse_level (root, flags, depth, func, data);
      break;
    }
}

static gboolean
g_node_find_func (GNode	  *node,
		  gpointer data)
{
  register gpointer *d = data;
  
  if (*d != node->data)
    return FALSE;
  
  *(++d) = node;
  
  return TRUE;
}

GNode*
g_node_find (GNode	       *root,
	     GTraverseType	order,
	     GTraverseFlags	flags,
	     gpointer		data)
{
  gpointer d[2];
  
  g_return_val_if_fail (root != NULL, NULL);
  g_return_val_if_fail (order <= G_LEVEL_ORDER, NULL);
  g_return_val_if_fail (flags <= G_TRAVERSE_MASK, NULL);
  
  d[0] = data;
  d[1] = NULL;
  
  g_node_traverse (root, order, flags, -1, g_node_find_func, d);
  
  return d[1];
}

static void
g_node_count_func (GNode	 *node,
		   GTraverseFlags flags,
		   guint	 *n)
{
  if (node->children)
    {
      GNode *child;
      
      if (flags & G_TRAVERSE_NON_LEAFS)
	(*n)++;
      
      child = node->children;
      while (child)
	{
	  g_node_count_func (child, flags, n);
	  child = child->next;
	}
    }
  else if (flags & G_TRAVERSE_LEAFS)
    (*n)++;
}

guint
g_node_n_nodes (GNode	      *root,
		GTraverseFlags flags)
{
  guint n = 0;
  
  g_return_val_if_fail (root != NULL, 0);
  g_return_val_if_fail (flags <= G_TRAVERSE_MASK, 0);
  
  g_node_count_func (root, flags, &n);
  
  return n;
}

GNode*
g_node_last_child (GNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);
  
  node = node->children;
  if (node)
    while (node->next)
      node = node->next;
  
  return node;
}

GNode*
g_node_nth_child (GNode *node,
		  guint	 n)
{
  g_return_val_if_fail (node != NULL, NULL);
  
  node = node->children;
  if (node)
    while ((n-- > 0) && node)
      node = node->next;
  
  return node;
}

guint
g_node_n_children (GNode *node)
{
  guint n = 0;
  
  g_return_val_if_fail (node != NULL, 0);
  
  node = node->children;
  while (node)
    {
      n++;
      node = node->next;
    }
  
  return n;
}

GNode*
g_node_find_child (GNode	 *node,
		   GTraverseFlags flags,
		   gpointer	  data)
{
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (flags <= G_TRAVERSE_MASK, NULL);
  
  node = node->children;
  while (node)
    {
      if (node->data == data)
	{
	  if (G_NODE_IS_LEAF (node))
	    {
	      if (flags & G_TRAVERSE_LEAFS)
		return node;
	    }
	  else
	    {
	      if (flags & G_TRAVERSE_NON_LEAFS)
		return node;
	    }
	}
      node = node->next;
    }
  
  return NULL;
}

gint
g_node_child_position (GNode *node,
		       GNode *child)
{
  register guint n = 0;
  
  g_return_val_if_fail (node != NULL, -1);
  g_return_val_if_fail (child != NULL, -1);
  g_return_val_if_fail (child->parent == node, -1);
  
  node = node->children;
  while (node)
    {
      if (node == child)
	return n;
      n++;
      node = node->next;
    }
  
  return -1;
}

gint
g_node_child_index (GNode   *node,
		    gpointer data)
{
  register guint n = 0;
  
  g_return_val_if_fail (node != NULL, -1);
  
  node = node->children;
  while (node)
    {
      if (node->data == data)
	return n;
      n++;
      node = node->next;
    }
  
  return -1;
}

GNode*
g_node_first_sibling (GNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);
  
  if (node->parent)
    return node->parent->children;
  
  while (node->prev)
    node = node->prev;
  
  return node;
}

GNode*
g_node_last_sibling (GNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);
  
  while (node->next)
    node = node->next;
  
  return node;
}

void
g_node_children_foreach (GNode		 *node,
			 GTraverseFlags	  flags,
			 GNodeForeachFunc func,
			 gpointer	  data)
{
  g_return_if_fail (node != NULL);
  g_return_if_fail (flags <= G_TRAVERSE_MASK);
  g_return_if_fail (func != NULL);
  
  node = node->children;
  while (node)
    {
      register GNode *current;
      
      current = node;
      node = current->next;
      if (G_NODE_IS_LEAF (current))
	{
	  if (flags & G_TRAVERSE_LEAFS)
	    func (current, data);
	}
      else
	{
	  if (flags & G_TRAVERSE_NON_LEAFS)
	    func (current, data);
	}
    }
}

#define __G_NODE_C__
#include "galiasdef.c"
