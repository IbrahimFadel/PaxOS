#ifndef KERNEL_PIC_H
#define KERNEL_PIC_H

#include <stdint.h>

void pic_remap(int offset1, int offset2);
void pic_mask_irq(uint8_t irq);
void pic_unmask_irq(uint8_t irq);
void pic_unmask_all(void);
void pic_mask_all(void);
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);

#endif
