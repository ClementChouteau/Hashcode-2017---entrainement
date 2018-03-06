glouton: clean
	g++ -std=c++11 -O3 -march=native glouton.cpp -o bin/glouton

out_glouton: glouton
	./bin/glouton < in/example.in > out/example.out
	./bin/glouton < in/small.in > out/small.out
	./bin/glouton < in/medium.in > out/medium.out
	./bin/glouton < in/big.in > out/big.out	

progdyn: clean
	g++ -std=c++11 -O3 -march=native progdyn.cpp -o bin/progdyn

out_progdyn: progdyn
	./bin/progdyn < in/example.in > out/example.out
	./bin/progdyn < in/small.in > out/small.out
	./bin/progdyn < in/medium.in > out/medium.out
	./bin/progdyn < in/big.in > out/big.out	

progdyn_bloc: clean
	g++ -std=c++11 -O3 -march=native progdyn_bloc.cpp -o bin/progdyn_bloc

out_progdyn_bloc: progdyn_bloc
	./bin/progdyn_bloc < in/example.in > out/example.out
	./bin/progdyn_bloc < in/small.in > out/small.out
	./bin/progdyn_bloc < in/medium.in > out/medium.out
	./bin/progdyn_bloc < in/big.in > out/big.out	
	
clean:
	rm -f bin/glouton
	rm -f bin/progdyn
	rm -f out/example.out
	rm -f out/small.out
	rm -f out/medium.out
	rm -f out/big.out

