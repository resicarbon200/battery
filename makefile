CXX :=g++
# インクルードファイル等
CXXFLAGS :=-std=c++11 `pkg-config --cflags opencv4` `pkg-config --libs opencv4` -lwiringPi

# ソースディレクトリの中を(shellの)findコマンドで走破してサブディレクトリまでリスト化する
SRCDIRS :=$(shell find ./ -type d)
# ソースディレクトリを元にforeach命令で全cppファイルをリスト化する
SOURCES :=$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))
# C++ファイルの.cppを.oに変えたものをオブジェクトファイルの名前とする
OBJECTS :=$(SOURCES:.cpp=.o)
# C++ファイルの.cppを.binに変えたものを実行ファイルの名前とする
TEST :=$(wildcard test/*.cpp)
BINARYS :=main.bin $(TEST:.cpp=.bin)
#BINARYS :=test/convexhull.bin main.bin


all: $(BINARYS) $(OBJECTS)

main.bin: main.o class/Marker.o class/PosMarker.o
	$(CXX) -w $^ $(CXXFLAGS) -o $@

%.bin: %.o
	$(CXX) -w $^ $(CXXFLAGS) -o $@

%.o: %.cpp
	$(CXX) -w -c $< $(CXXFLAGS) -o $@

clean:
	rm -rf $(BINARYS) $(OBJECTS)

echo:
	echo $(SOURCES)
	echo $(BINARYS)
