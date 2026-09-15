PREFIX ?= /usr/local
SYSCONFDIR ?= /etc
CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -Wpedantic -std=c11
CPPFLAGS += $(shell pkg-config --cflags x11 xrandr)
LDLIBS += $(shell pkg-config --libs x11 xrandr)

TARGET := mate-snap-wm
SOURCE := src/mate-snap-wm.c

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDLIBS)

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(SYSCONFDIR)/xdg/autostart
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	install -m 644 share/autostart/mate-snap-wm.desktop $(DESTDIR)$(SYSCONFDIR)/xdg/autostart/mate-snap-wm.desktop

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	rm -f $(DESTDIR)$(SYSCONFDIR)/xdg/autostart/mate-snap-wm.desktop

clean:
	rm -f $(TARGET)