include ./config.mk

srctree := .
objtree := .

real_srctree := $(srctree)
real_objtree := $(objtree)

# This is our default target - The default is the first target in the file so
# we need to define this fairly high-up.
all: real-all

PHONY += all install clean dist real-all configure

# Predefine this variable. It contains a list of extra files to clean. Ex.
CLEAN_LIST :=

# List of files that dependency files should be generated for
DEPS :=

# Current project being compiled - Blank for core
PROJ :=
EXES :=
SRC_OBJS :=

# Set configuration options
Q := @
quiet := quiet
ifeq ($(V),y)
	quiet :=
endif

ifdef silent
	quiet := silent
endif

_echo_cmd = echo $(2)
quiet_echo_cmd = echo $(1)
slient_echo_cmd = true

mecho = $(call $(quiet)_echo_cmd,$(1),$(2))


# This includes everything in the 'include' folder of the $(objtree)
# This is so that the code can reference generated include files
CPPFLAGS += -I'$(objtree)/include/'

define create_link_rule
$(1): $(2)
	@$$(call mecho," LD      $$@","$$(LD) -r $(2) -o $$@")
	$$(Q)$$(LD) -r $(2) -o $$@
endef

define create_cc_rule
ifneq ($$(wildcard $(2)),)
$(1): $(2)
	@$$(call mecho," CC      $$@","$$(CC) $$(CFLAGS) $$(CPPFLAGS) $(3) -c $$< -o $$@")
	$$(Q)$$(CC) $$(CFLAGS) $$(CPPFLAGS) $(3) -c $$< -o $$@

$(dir $(1)).$(basename $(notdir $(1))).d: $(2)
	@$$(call mecho," CCDEP   $$@","$$(CC) -MM -MP -MF $$@ $$(CPPFLAGS) $$(CFLAGS) $(3) $$< -MT $$(objtree)/$$*.o -MT $$@")
	$$(Q)$$(CC) -MM -MP -MF $$@ $$(CPPFLAGS) $$(CFLAGS) $(3) $$< -MT $(1) -MT $$@
endif
endef

subdir_level := 0

# Traverse into tree
define subdir_inc
objtree := $$(objtree)/$(1)
srctree := $$(srctree)/$(1)

subdir_level_prev := $$(subdir_level)
subdir_level := $$(shell expr $$(subdir_level) + 1)

cflags-$$(subdir_level) := $$(cflags-$$(subdir_level_prev))

subdir-y :=
objs-y :=
clean-list-y :=
cflags-y :=

_tmp := $$(shell mkdir -p $$(objtree))
include $$(srctree)/Makefile

cflags-$$(subdir_level) += $$(cflags-y)

cflags-y := $$(cflags-$$(subdir_level))

CLEAN_LIST += $$(patsubst %,$$(objtree)/%,$$(objs-y)) $$(patsubst %,$$(objtree)/%,$$(clean-list-y)) $$(objtree).o
DEPS += $$(patsubst %,$$(objtree)/%,$$(objs-y))

objs := $$(patsubst %,$$(objtree)/%,$$(objs-y)) $$(patsubst %,$$(objtree)/%.o,$$(subdir-y))

$$(foreach obj,$$(patsubst %,$$(objtree)/%,$$(objs-y)),$$(eval $$(call create_cc_rule,$$(obj),$$(obj:.o=.c),$$($$(PROJ)_CFLAGS) $$(cflags-y))))

$$(eval $$(call create_link_rule,$$(objtree).o,$$(objs)))

$$(foreach subdir,$$(subdir-y),$$(eval $$(call subdir_inc,$$(subdir))))

subdir_level_prev := $$(subdir_level);
subdir_level := $$(shell expr $$(subdir_level) - 1)

cflags-$(subdir_level) := $$(cflags-$$(subdir_level_prev))
srctree := $$(patsubst %/$(1),%,$$(srctree))
objtree := $$(patsubst %/$(1),%,$$(objtree))
endef


define proj_ccld_rule
$(1): $(2) | $$(objtree)/bin
	@$$(call mecho," CCLD    $$@","$$(CC) $(3) $(2) -o $$@ $(4)")
	$$(Q)$$(CXX) $$(LDFLAGS) $(3) $(2) -o $$@ $(4)
endef

define proj_inc
include $(1)/config.mk
PROG := $$(objtree)/bin/$$(EXE)
PROJ := $$(EXEC)

objs := $$(sort $$($$(EXEC)_OBJS) $$(SRC_OBJS))

$$(eval $$(call proj_ccld_rule,$$(PROG),$$(objs),$$($$(EXEC)_CFLAGS),$$($$(EXEC)_LIBFLAGS)))
$$(eval $$(call subdir_inc,$$(EXE)))
CLEAN_LIST += $$(PROG)
endef

ifeq ($(CONFIG_DEBUG),y)
	CPPFLAGS += -DCONFIG_DEBUG
	CFLAGS += -g
	ASFLAGS += -g
	LDFLAGS += -g
endif

ifeq ($(CONFIG_PROF),y)
	CFLAGS += -pg
	LDFLAGS += -pg
endif

$(eval $(call proj_inc,rubik))
CLEAN_LIST += $(objtree)/bin

EXES := $(objtree)/bin/rubik

# Actual entry
real-all: $(EXES)

install:
	$(Q)mkdir -p $(BINDIR)
	@echo " INSTALL cboy"
	$(Q)install -m 775 ./bin/nclyr $(BINDIR)
	@echo " cboy Installation done"

dist: clean
	$(Q)mkdir -p $(EXE)-$(VERSION_N)
	$(Q)cp -R Makefile README.md config.mk LICENSE ./doc ./include ./src ./cboy $(EXE)-$(VERSION_N)
	$(Q)tar -cf $(EXE)-$(VERSION_N).tar $(EXE)-$(VERSION_N)
	$(Q)gzip $(EXE)-$(VERSION_N).tar
	$(Q)rm -fr $(EXE)-$(VERSION_N)
	@$(call mecho," Created $(EXE)-$(VERSION_N).tar.gz","gzip $(EXE)-$(VERSION_N).tar")

clean:
	$(Q)for file in $(CLEAN_LIST); do \
		if [ -e $$file ]; then \
		    $(call mecho," RM      $$file";,"rm -fr $$file";) \
			rm -rf $$file; \
		fi \
	done

$(objtree)/bin:
	@$(call mecho," MKDIR   $@","$(MKDIR) $@")
	$(Q)$(MKDIR) $@

$(objtree)/%.o: $(srctree)/%.cpp
	@$(call mecho," C++     $@","$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@")
	$(Q)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(objtree)/%.o: $(srctree)/%.c
	@$(call mecho," CC      $@","$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@")
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(objtree)/.%.d: $(srctree)/%.c
	@$(call mecho," CCDEP   $@","$(CC) -MM -MP -MF $@ $(CPPFLAGS) $(CFLAGS) $< -MT $(objtree)/$*.o -MT $@")
	$(Q)$(CC) -MM -MP -MF $@ $(CPPFLAGS) $(CFLAGS) $< -MT $(objtree)/$*.o -MT $@

$(objtree)/.%.d: $(srctree)/%.cpp
	@$(call mecho," CXXCDEP $@","$(CXX) -MM -MP -MF $@ $(CPPFLAGS) $(CXXFLAGS) $< -MT $(objtree)/$*.o -MT $@")
	$(Q)$(CXX) -MM -MP -MF $@ $(CPPFLAGS) $(CXXFLAGS) $< -MT $(objtree)/$*.o -MT $@

DEP_LIST := $(foreach dep,$(DEPS),$(dir $(dep)).$(notdir $(dep)))
DEP_LIST := $(DEP_LIST:.o=.d)

real-all: $(DEP_LIST)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEP_LIST)
endif
CLEAN_LIST += $(DEP_LIST)

.PHONY: $(PHONY)

