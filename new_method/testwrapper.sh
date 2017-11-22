ln -s .libs build
cd tests
LD_LIBRARY_PATH=../.libs ./run_tests.py
RET=$?
cd ..
rm -f build
exit $RET
