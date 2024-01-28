CC=gcc -lstdc++
CXXFLAGS=-Wall -Werror -Wextra -std=c++17

UNAME_S:=$(shell uname -s)
ifeq ($(UNAME_S),Linux)
	OPEN_CMD=xdg-open
	LEAKS_CMD=valgrind --tool=memcheck --leak-check=yes
	LIBS=-lgtest -lpthread -lm -lstdc++
	GCOV_FLAGS=
	APP_NAME=smartcalc
endif

ifeq ($(UNAME_S),Darwin)
	OPEN_CMD=open
	LEAKS_CMD=leaks --atExit --
	LIBS=-lgtest -lm
	GCOV_FLAGS=--ignore-errors mismatch
	APP_NAME=smartcalc.app
endif

all: gui

gui: 
	qmake -o qt.mk
	make -f qt.mk all

tests: clean
	$(CC) $(CXXFLAGS) $(LIBS) -c model/calculator.cc
	$(CC) $(CXXFLAGS) $(LIBS) calculator.o tests/calculator_model_test.cc -o test
	$(LEAKS_CMD) ./test

gcov: 
	$(CC) $(CXXFLAGS) $(LIBS) --coverage -c model/calculator.cc
	$(CC) $(CXXFLAGS) $(LIBS) --coverage calculator.o tests/calculator_model_test.cc -o test
	./test
	rm -rf test_main.gcda test_main.gcno
	lcov -t "s21_containers_test" -o fizzbuzz.info -c -d . $(GCOV_FLAGS)
	genhtml -o gcov_report fizzbuzz.info
	$(OPEN_CMD) gcov_report/index.html

install: gui
	mkdir -p build
	cp -r $(APP_NAME) build

uninstall:
	rm -rf build

dist:
	mkdir -p dist
	tar cvzf smartcalc.tgz model view controller qcustomplot tests Makefile smartcalc.pro main.cc docs
	mv smartcalc.tgz dist

dvi: 
	mkdir -p build
	cp -r docs build

clean:
	rm -rf *.o
	rm -rf */*.o
	rm -rf *.gcda
	rm -rf *.gcno
	rm -rf *.dSYM
	rm -rf vgcore.*
	rm -rf *.info
	rm -rf .qmake.stash
	rm -rf moc_*
	rm -rf qt.mk
	rm -rf .cache
	rm -rf .tmp
	rm -rf gcov_report
	rm -rf test
	rm -rf $(APP_NAME)
	rm -rf qrc_icons.cpp
	rm -rf dist
