#ifndef _H_SM_NONCE_H_
#define _H_SM_NONCE_H_

#include <sbi/sbi_types.h>
//unsigned long sbi_sm_track_nonce(unsigned long nonce);
//unsigned long sbi_sm_verify_nonce(unsigned long nonce);
int add_tracking_entry(unsigned long nonce);
int verify_nonce(unsigned long nonce, enclave_id eid);

#endif

