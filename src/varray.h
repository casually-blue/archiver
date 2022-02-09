#pragma once

typedef struct varray_t* Varray;

Varray varray_new(unsigned int initial_storage_length);
Varray varray_delete(Varray v);

int varray_count(Varray v);

void* varray_get(Varray v, unsigned int index);
int varray_append(Varray v, void* val);

