build_dir="build"
if [ ! -d "$build_dir" ]; then
  mkdir $build_dir
fi

cd $build_dir

if [ -z $1 ]; then
      #echo $1 "is empty"
	  win_build_dir="x64"
	  config=" Win64"
else
      #echo $1 "is NOT empty"
	  win_build_dir="x86"
	  config=""
fi

if [ ! -d "$win_build_dir" ]; then
  mkdir $win_build_dir
fi

cd $win_build_dir
cmake ../.. -G"Visual Studio 15 2017$config"
