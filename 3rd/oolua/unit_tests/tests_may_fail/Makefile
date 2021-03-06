# GNU Make project makefile autogenerated by Premake
ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifndef CC
  CC = gcc
endif

ifndef CXX
  CXX = g++
endif

ifndef AR
  AR = ar
endif

ifeq ($(config),debug)
  OBJDIR     = ../../obj/Debug/tests_may_fail
  TARGETDIR  = ../../bin/Debug
  TARGET     = $(TARGETDIR)/tests_may_fail
  DEFINES   += -DDEBUG -D_DEBUG -DPLATFORM_CHECKED -DUNIX_BUILD
  INCLUDES  += -Iinclude/cppunit -Iinclude/lua -Iinclude -I../../include -I/usr/local/include -I/usr/include -I../test_classes
  CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -g -Wall -W -Wall -ansi -pedantic -std=c++98
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -Lusr/local/lib -Lusr/lib -L../../bin/Debug
  LIBS      += -loolua_d -lgmock -lpthread -lcppunit -llua -ldl
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += ../../bin/Debug/liboolua_d.a
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
	@echo Running post-build commands
	$(TARGET)
  endef
endif

ifeq ($(config),release)
  OBJDIR     = ../../obj/Release/tests_may_fail
  TARGETDIR  = ../../bin/Release
  TARGET     = $(TARGETDIR)/tests_may_fail
  DEFINES   += -DNDEBUG -DRELEASE -DPLATFORM_CHECKED -DUNIX_BUILD
  INCLUDES  += -Iinclude/cppunit -Iinclude/lua -Iinclude -I../../include -I/usr/local/include -I/usr/include -I../test_classes
  CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -O2 -Wall -W -Wall -ansi -pedantic -std=c++98
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -s -Lusr/local/lib -Lusr/lib -L../../bin/Release
  LIBS      += -loolua -lgmock -lpthread -lcppunit -llua -ldl
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += ../../bin/Release/liboolua.a
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
	@echo Running post-build commands
	$(TARGET)
  endef
endif

OBJECTS := \
	$(OBJDIR)/push_pull_may_fail.o \
	$(OBJDIR)/main.o \

RESOURCES := \

SHELLTYPE := msdos
ifeq (,$(ComSpec)$(COMSPEC))
  SHELLTYPE := posix
endif
ifeq (/bin,$(findstring /bin,$(SHELL)))
  SHELLTYPE := posix
endif

.PHONY: clean prebuild prelink

all: $(TARGETDIR) $(OBJDIR) prebuild prelink $(TARGET)
	@:

$(TARGET): $(GCH) $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking tests_may_fail
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning tests_may_fail
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild:
	$(PREBUILDCMDS)

prelink:
	$(PRELINKCMDS)

ifneq (,$(PCH))
$(GCH): $(PCH)
	@echo $(notdir $<)
	-$(SILENT) cp $< $(OBJDIR)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
endif

$(OBJDIR)/push_pull_may_fail.o: push_pull_may_fail.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/main.o: main.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
