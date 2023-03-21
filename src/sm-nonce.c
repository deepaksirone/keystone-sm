#include "sm-nonce.h"

static spinlock_t nonce_lock = SPIN_LOCK_INITIALIZER;
static unsigned long verifiable_nonces[MAX_VERIFIABLE_NONCES];
//static enclave_id nonce_to_enclave_ids[MAX_VERIFIABLE_NONCES][ENCL_MAX];
static char nonces_to_hashes[MAX_VERIFIABLE_NONCES][ENCL_MAX][64];

int add_tracking_entry(unsigned long nonce)
{
	spin_lock(&nonce_lock);
	int free_idx = -1;

	for (int i = 0; i < MAX_VERIFIABLE_NONCES; i++) {
		if (verifiable_nonces[i] == nonce && nonce != 0) {
			return -1;
		}
	}


	for (int i = 0; i < MAX_VERIFIABLE_NONCES; i++) {
		if (verifiable_nonces[i] == 0) {
			free_idx = i;
		}
	}

	//evict a nonce entry from evict_idx
	if (free_idx == -1) {
		//clear all hashes for this entry
		for (int i = 0; i < ENCL_MAX; i++) {
			for (int j = 0; j < 64; j++) {
				nonces_to_hashes[evict_idx][i][j] = 0;
			}
		}

		free_idx = evict_idx;
		evict_idx = (evict_idx + 1) % MAX_VERIFIABLE_NONCES;
	}

	verifiable_nonces[free_idx] = nonce;

	spin_unlock(&nonce_lock);
}

int verify_nonce(unsigned long nonce, enclave_id eid)
{
	spin_lock(&nonce_lock);
	spin_lock(&encl_lock);
	
	int nonce_idx = -1;
	for (int i = 0; i < MAX_VERIFIABLE_NONCES; i++) {
		if (verifiable_nonces[i] == nonce) {
			nonce_idx = i;
		}
	}

	if (nonce_idx == -1)
		return 0;

	int found_hash = 0;
	int all_zeroes_idx = 100000;
	for (int i = 0; i < ENCL_MAX; i++) {
		int match = 1;
		int all_zeroes = 1;
		for (int j = 0; j < 64; j++) {
			if (nonces_to_hashes[nonce_idx][i][j] != enclaves[eid].hash[j]) {
				match = 0;
			}

			if (nonces_to_hashes[nonce_idx][i][j] != 0) {
				all_zeroes = 0;
			}
		}

		if (match) {
			return 0;
		}

		if (all_zeroes) {
			if (all_zeroes_idx == 100000) {
				all_zeroes_idx = i;
			}
		}
	}

	if (all_zeroes_idx == 100000) {
		return 0;
	}

	//Insert hash into empty slot
	for(int j = 0; j < 64; j++)
		nonces_to_hashes[nonce_idx][all_zeroes_idx][j] = enclaves[eid].hash[j];

	spin_unlock(&encl_lock);
	spin_unlock(&nonce_hash);

	return 1;

}
