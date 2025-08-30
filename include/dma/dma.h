
#pragma once 


void dma_enable(uint32_t enb);
void dma0_clear(uint32_t dst_addr, uint32_t length);
void dma0_copy(uint32_t src_addr, uint32_t dst_addr, uint32_t length);
void dma0_copy_2d(uint32_t src_addr, uint32_t src_stride, uint32_t dst_addr, uint32_t dst_stride, uint32_t x_length, uint32_t y_length);
