/*
 * GJB5369: 4.1.2.2
 * avoid using the function as parameter
 */

void test_p(int p_1) {}

/* Using function as parameter is forbidden */
void static_p(void(*p_proc_pointer)(int parameter_1)) {
    p_proc_pointer = test_p;
}

