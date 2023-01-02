bin/splc test/test_3_r01.spl
bin/splc test/test_3_r02.spl
bin/splc test/test_3_r03.spl
bin/splc test/test_3_r04.spl
bin/splc test/test_3_r05.spl
bin/splc test/test_3_r06.spl
bin/splc test/test_3_r07.spl
bin/splc test/test_3_r08.spl
bin/splc test/test_3_r09.spl
bin/splc test/test_3_r10.spl

bin/splc test-extra/test_3_b01.spl
bin/splc test-extra/test_3_b02.spl
bin/splc test-extra/test_3_b03.spl
bin/splc test-extra/test_3_b04.spl
bin/splc test-extra/test_struct.spl

gcc -x c test-cpp/test_3_r01.c -o test-cpp/test_3_r01.out
gcc -x c test-cpp/test_3_r02.c -o test-cpp/test_3_r02.out
gcc -x c test-cpp/test_3_r03.c -o test-cpp/test_3_r03.out
gcc -x c test-cpp/test_3_r04.c -o test-cpp/test_3_r04.out
gcc -x c test-cpp/test_3_r05.c -o test-cpp/test_3_r05.out
gcc -x c test-cpp/test_3_r06.c -o test-cpp/test_3_r06.out
gcc -x c test-cpp/test_3_r07.c -o test-cpp/test_3_r07.out
gcc -x c test-cpp/test_3_r08.c -o test-cpp/test_3_r08.out
gcc -x c test-cpp/test_3_r09.c -o test-cpp/test_3_r09.out
gcc -x c test-cpp/test_3_r10.c -o test-cpp/test_3_r10.out

gcc -x c test-cpp/test_3_b01.c -o test-cpp/test_3_b01.out
gcc -x c test-cpp/test_3_b02.c -o test-cpp/test_3_b02.out
gcc -x c test-cpp/test_3_b03.c -o test-cpp/test_3_b03.out
gcc -x c test-cpp/test_3_b04.c -o test-cpp/test_3_b04.out
gcc -x c test-cpp/test_struct.c -o test-cpp/test_struct.out
