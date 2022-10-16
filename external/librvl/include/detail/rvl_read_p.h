#ifndef RVL_READ_P_H
#define RVL_READ_P_H

#include "rvl_p.h"

void rvl_read_chunk_header (RVL *self, RVLSize *size, RVLChunkCode *code);
void rvl_read_chunk_payload (RVL *self, RVLByte *data, RVLSize size);

void rvl_read_VHDR_chunk (RVL *self, RVLConstByte* rbuf, RVLSize size);
void rvl_read_GRID_chunk (RVL *self, RVLConstByte* rbuf, RVLSize size);
void rvl_read_DATA_chunk (RVL *self, RVLConstByte* rbuf, RVLSize size);
void rvl_read_TEXT_chunk (RVL *self, RVLConstByte* rbuf, RVLSize size);

void rvl_read_file_sig (RVL *self);
void rvl_read_data (RVL *self, RVLByte *data, RVLSize size);
void rvl_read_data_default (RVL *self, RVLByte *data, RVLSize size);

#endif
