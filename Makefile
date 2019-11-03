SAN       =
CFLAGS   += -O2 -fPIC -flto $(SAN)
CXXFLAGS += $(CFLAGS) -std=c++17 -I /usr/include/x86_64-linux-gnu/qt5
LD_FLAGS += -s -fPIC -flto $(SAN)

PKGCONFIG = pkg-config
PACKAGES = Qt5Core Qt5Widgets Qt5Gui
QT_CFLAGS = $(shell $(PKGCONFIG) --cflags $(PACKAGES))
QT_LDFLAGS = $(shell $(PKGCONFIG) --libs $(PACKAGES))
QT_MOCBIN = $(shell $(PKGCONFIG) --variable=host_bins Qt5Core)/moc

.PHONY: all clean

all: game_cli game_qt

clean:
	rm -f *.o moc_*.cc game_cli game_qt

game_cli: game_cli.o game.o
	$(CXX) -o $@ $^ $(LD_FLAGS)

game_qt: game_qt.o game.o game_window.o game_tile.o game_keygrabber.o moc_game_window.o moc_game_tile.o moc_game_keygrabber.o 
	$(CXX) -o $@ $^ $(LD_FLAGS) $(QT_LDFLAGS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

moc_%.cc: %.h
	$(QT_MOCBIN) -o $@ $<

game.o: game.cc game.h
game_keygrabber.o: game_keygrabber.cc game_keygrabber.h
game_tile.o: game_tile.cc game_tile.h game.h
game_window.o: game_window.cc game_window.h game.h game_tile.h game_keygrabber.h
game_cli.o: game_cli.cc game.h
game_qt.o: game_qt.cc game.h game_window.h game_tile.h game_keygrabber.h
