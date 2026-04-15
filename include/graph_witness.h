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

// The returned *wtns_data must be freed with gw_free_witness (NOT free),
// because the buffer is owned by Rust's global allocator, which may differ
// from libc's.
int
gw_calc_witness_prepared(const void *handle,
                         const char *inputs,
                         void **wtns_data, size_t *wtns_len,
                         gw_status_t *status);

// Compute witness and return raw little-endian field-element bytes, without
// the WTNS file-format wrapper. *fe_data points to fe_num_elements * fe_size
// bytes where fe_size is 32 for bn254 (derivable from the graph). Free via
// gw_free_witness(*fe_data, fe_num_elements * fe_size).
int
gw_calc_witness_raw_prepared(const void *handle,
                             const char *inputs,
                             void **fe_data, size_t *fe_num_elements,
                             gw_status_t *status);

// Wrap raw little-endian field-element bytes into WTNS v2 bytes. Inverse of
// gw_calc_witness_raw_prepared's output → WTNS. Free *wtns_data via
// gw_free_witness.
int
gw_wtns_from_raw(const void *handle,
                 const void *fe_data, size_t fe_num_elements,
                 void **wtns_data, size_t *wtns_len,
                 gw_status_t *status);

void
gw_free_witness(void *ptr, size_t len);

void
gw_free_graph(void *handle);

void
gw_free_status(gw_status_t *status) {
  if (status->error_msg != NULL) {
	free(status->error_msg);
  }
}

#endif // RUST_GRAPH_WITNESS_H
