FROM registry.gitlab.gnome.org/gnome/glib/fedora:v13

ARG COVERITY_SCAN_PROJECT_NAME
ARG COVERITY_SCAN_TOKEN

RUN curl https://scan.coverity.com/download/cxx/linux64 \
    -o /tmp/cov-analysis-linux64.tgz \
    --form project="${COVERITY_SCAN_PROJECT_NAME}" \
    --form token="${COVERITY_SCAN_TOKEN}" \
 && tar xfz /tmp/cov-analysis-linux64.tgz \
 && rm /tmp/cov-analysis-linux64.tgz
