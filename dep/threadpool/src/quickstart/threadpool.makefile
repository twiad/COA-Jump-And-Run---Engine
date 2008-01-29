cpp_comp = g++
boost_home=../../../boost
threadpool_home=../..

# ------------------------------------------
# Header files used
# ------------------------------------------
boost_include_dir=-I"$(boost_home)"
threadpool_include_dir=-I"$(threadpool_home)/include"
all_include_dirs = $(boost_include_dir) $(threadpool_include_dir)

# ------------------------------------------
# Libraries used
# ------------------------------------------
boost_thread_lib = -L/usr/local/lib -lboost_thread-gcc-mt-d-1_33_1
