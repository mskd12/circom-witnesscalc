#include <stddef.h>
#include <stdlib.h>

#ifndef RUST_GRAPH_WITNESS_H
#define RUST_GRAPH_WITNESS_H

typedef enum {
  OK = 0,
  ERROR = 1
} GW_ERROR_CODE;

typedef struct {
  GW_ERROR_CODE code;
  char *error_msg;
} gw_status_t;

int
gw_calc_witness(const char *inputs,
				const void *graph_data, const size_t graph_data_len,
			    void **wtns_data, size_t *wtns_len,
				const gw_status_t *status);

// Parse a graph binary once and return an opaque handle in *handle_out. The
// handle can be reused across many gw_calc_witness_prepared calls; this
// avoids repaying the per-call graph deserialization cost (hundreds of ms
// for larger circuits). Free via gw_free_graph.
int
gw_prepare_graph(const void *graph_data, size_t graph_data_len,
                 void **handle_out,
                 gw_status_t *status);

int
gw_calc_witness_prepared(const void *handle,
                         const char *inputs,
                         void **wtns_data, size_t *wtns_len,
                         gw_status_t *status);

void
gw_free_graph(void *handle);

void
gw_free_status(gw_status_t *status) {
  if (status->error_msg != NULL) {
	free(status->error_msg);
  }
}

#endif // RUST_GRAPH_WITNESS_H
