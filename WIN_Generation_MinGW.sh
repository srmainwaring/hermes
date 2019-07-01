build_dir="build"
if [ ! -d "$build_dir" ]; then
  mkdir $build_dir
fi

cd $build_dir

win_build_dir="MinGW"

if [ ! -d "$win_build_dir" ]; then
  mkdir $win_build_dir
fi

cd $win_build_dir
cmake ../.. -G"MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND"
mingw32-make -j $(nproc)
