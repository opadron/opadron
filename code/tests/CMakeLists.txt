
add_executable(id_allocator id_allocator.c)
target_link_libraries(id_allocator code)

add_executable(mem_stack mem_stack.c)
target_link_libraries(mem_stack code)

add_executable(tic tic.c)
target_link_libraries(tic code)

add_executable(trie trie.c)
target_link_libraries(trie code)

add_test(id_allocator id_allocator)
add_test(mem_stack mem_stack)
add_test(tic tic)
add_test(trie trie)


