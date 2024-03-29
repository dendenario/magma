// Magma.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <stdint.h>
#include <stdio.h>
#include <memory.h>
#include <iostream>

using namespace std;

typedef uint8_t substitution_t[128];

substitution_t pi = {
	0xc, 0x4, 0x6, 0x2, 0xa, 0x5, 0xb, 0x9, 0xe, 0x8, 0xd, 0x7, 0x0, 0x3, 0xf, 0x1,
	0x6, 0x8, 0x2, 0x3, 0x9, 0xa, 0x5, 0xc, 0x1, 0xe, 0x4, 0x7, 0xb, 0xd, 0x0, 0xf,
	0xb, 0x3, 0x5, 0x8, 0x2, 0xf, 0xa, 0xd, 0xe, 0x1, 0x7, 0x4, 0xc, 0x9, 0x6, 0x0,
	0xc, 0x8, 0x2, 0x1, 0xd, 0x4, 0xf, 0x6, 0x7, 0x0, 0xa, 0x5, 0x3, 0xe, 0x9, 0xb,
	0x7, 0xf, 0x5, 0xa, 0x8, 0x1, 0x6, 0xd, 0x0, 0x9, 0x3, 0xe, 0xb, 0x4, 0x2, 0xc,
	0x5, 0xd, 0xf, 0x6, 0x9, 0x2, 0xc, 0xa, 0xb, 0x7, 0x8, 0x1, 0x4, 0x3, 0xe, 0x0,
	0x8, 0xe, 0x2, 0x5, 0x6, 0x9, 0x1, 0xc, 0xf, 0x4, 0xb, 0x0, 0xd, 0xa, 0x3, 0x7,
	0x1, 0x7, 0xe, 0xd, 0x0, 0x5, 0x8, 0x3, 0x4, 0xf, 0xa, 0x6, 0x9, 0xc, 0xb, 0x2
};

void magma_round(uint32_t round_key, uint32_t* a1, uint32_t a0)
{
	uint32_t g = a0 + round_key;

	uint32_t t =
		((pi[0 + ((g & 0x0000000f) >> 0)]) << 0)
		| ((pi[16 + ((g & 0x000000f0) >> 4)]) << 4)
		| ((pi[32 + ((g & 0x00000f00) >> 8)]) << 8)
		| ((pi[48 + ((g & 0x0000f000) >> 12)]) << 12)
		| ((pi[64 + ((g & 0x000f0000) >> 16)]) << 16)
		| ((pi[80 + ((g & 0x00f00000) >> 20)]) << 20)
		| ((pi[96 + ((g & 0x0f000000) >> 24)]) << 24)
		| ((pi[112 + ((g & 0xf0000000) >> 28)]) << 28);

	*a1 ^= ((t << 11) | (t >> 21));
}

uint32_t* distribute_key(uint32_t* key)
{
	uint32_t* keys = new uint32_t[32];
	for (int i = 0; i < 8; i++)
	{
		keys[i] = key[i];
		keys[i + 8] = key[i];
		keys[i + 16] = key[i];
		keys[31 - i] = key[i];
	}
	return keys;
}

/* Зашифрование одного блока данных */
uint64_t magma_encrypt_block(uint32_t* key, uint64_t block)
{
	uint32_t a0 = block & 0xFFFFFFFF;
	uint32_t a1 = block >> 32;
	uint32_t a2 = 0;
	uint32_t* round_keys = distribute_key(key);
	for (int i = 0; i < 32; i++)
	{
		magma_round(round_keys[i], &a1, a0);
		a2 = a1;
		a1 = a0;
		a0 = a2;
	}
	return (((uint64_t)a0) << 32) ^ ((uint64_t)a1);
}

/* Расшифрование одного блока данных */
uint64_t magma_decrypt_block(uint32_t* key, uint64_t block)
{
	uint32_t a0 = block & 0xFFFFFFFF;
	uint32_t a1 = block >> 32;
	uint32_t a2 = 0;
	uint32_t* round_keys = distribute_key(key);
	for (int i = 0; i < 32; i++)
	{
		magma_round(round_keys[31-i], &a1, a0);
		a2 = a1;
		a1 = a0;
		a0 = a2;
	}
	return (((uint64_t)a0) << 32) ^ ((uint64_t)a1);
}
/* Тестирование корректности реализации операции зашифрования/расшифрования одного блока данных */
int test_magma_encrypt_decrypt_block()
{
	uint32_t key[8] = { 0xffeeddcc,0xbbaa9988,0x77665544,0x33221100,0xf0f1f2f3,0xf4f5f6f7,0xf8f9fafb,0xfcfdfeff };
	uint64_t block = 0xfedcba9876543210;
	uint64_t block2 = 0x4ee901e5c2d8ca3d;
	uint64_t encrypted = magma_encrypt_block(key,block);
	uint64_t decrypted = magma_decrypt_block(key, block2);
	cout << hex << encrypted << endl;
	if (encrypted == block2)
	{
		cout << "OK" << endl;
	}
	else
	{
		cout << "NOT OK" << endl;
	}
	cout << hex << decrypted << endl;
	if (decrypted == block)
	{
		cout << "OK" << endl;
	}
	else
	{
		cout << "NOT OK" << endl;
	}
	return 0;
}





int main()
{
	test_magma_encrypt_decrypt_block();
    return 0;
}

