#include "necronomicon.h"
#include "libds/hmap.h"

static hash_table_t* registry;

void init_registry() {
	registry = create_uint32_table();
	if (registry == NULL) {
		ERROR_MEM("failed to allocate registry table");
	}
}

bool has_decoration(uint32_t handle) {
	return table_contains(registry, (void*)(uintptr_t)handle);
}

struct decoration* get_decoration(uint32_t handle) {
	return (struct decoration*) table_get(registry, (void*)(uintptr_t)handle);
}

void register_decoration(struct decoration* decoration) {
	table_set(registry, (void*)(uintptr_t)decoration->handle, (void*)decoration);
}

void remove_decoration(uint32_t handle) {
	if (has_decoration(handle)) {
		struct decoration* decoration = get_decoration(handle);
		destroy_decoration(decoration);
		table_remove(registry, (void*)(uintptr_t)handle);
	}
}

void for_all(void (*func)(struct decoration* decoration)) {
	hash_it_t iterator = 0;
	struct decoration* decoration;
	while ((decoration = (struct decoration*) hash_it_next(registry, &iterator))) {
		func(decoration);
	}
}
