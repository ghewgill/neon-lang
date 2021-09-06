#ifndef BINARY_H
#define BINARY_H

struct tagTExecutor;

void binary_and32(struct tagTExecutor *exec);
void binary_bitCount32(struct tagTExecutor *exec);
void binary_extract32(struct tagTExecutor *exec);
void binary_get32(struct tagTExecutor *exec);
void binary_not32(struct tagTExecutor *exec);
void binary_or32(struct tagTExecutor *exec);
void binary_replace32(struct tagTExecutor *exec);
void binary_set32(struct tagTExecutor *exec);
void binary_shift_left32(struct tagTExecutor *exec);
void binary_shift_right32(struct tagTExecutor *exec);
void binary_shift_right_signed32(struct tagTExecutor *exec);
void binary_xor32(struct tagTExecutor *exec);

void binary_and64(struct tagTExecutor *exec);
void binary_bitCount64(struct tagTExecutor *exec);
void binary_extract64(struct tagTExecutor *exec);
void binary_get64(struct tagTExecutor *exec);
void binary_not64(struct tagTExecutor *exec);
void binary_or64(struct tagTExecutor *exec);
void binary_replace64(struct tagTExecutor *exec);
void binary_set64(struct tagTExecutor *exec);
void binary_shift_left64(struct tagTExecutor *exec);
void binary_shift_right64(struct tagTExecutor *exec);
void binary_shift_right_signed64(struct tagTExecutor *exec);
void binary_xor64(struct tagTExecutor *exec);

void binary_andBytes(struct tagTExecutor *exec);
void binary_bitCountBytes(struct tagTExecutor *exec);
void binary_notBytes(struct tagTExecutor *exec);
void binary_orBytes(struct tagTExecutor *exec);
void binary_xorBytes(struct tagTExecutor *exec);

#endif
