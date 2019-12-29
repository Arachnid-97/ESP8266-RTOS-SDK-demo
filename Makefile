#  copyright (c) 2010 Espressif System
#


# ifeq (<arg1>, <arg2>)比较参数 arg1 和 arg2 的值是否相同，相同为真 #
# ifneq (<arg1>, <arg2>)比较参数 arg1 和 arg2 的值是否相同，如果不同，则为真 #
# ifdef <variable-name>如果变量 <variable-name> 的值非空，那到表达式为真。否则，表达式为假 #
# ifndef <variable-name>如果变量 <variable-name> 的值空，那到表达式为真，和 ifdef 是相反的意思 #
#############################################################
# 条件判定

ifndef PDIR

endif
########################### END #############################


# AR：用于建立或修改备存文件，或是从备存文件中抽取文件 #
# CC：C语言编译程序 #
# NM：用来列出目标文件的符号清单 #
# CPP：C程序的预处理器（输出是标准输出设备） #
# OBJCOPY：将目标文件的一部分或者全部内容拷贝到另外一个目标文件中，或者实现目标文件的格式转换 #
# OBJDUMP：查看目标文件或者可执行的目标文件的构成 #
#############################################################
# 编译工具配置
# 根据 COMPILE选择编译器，从开头的脚本克制 Windows使用 xcc，Linux使用gcc。
# ESP8266 主要使用的编译器是 xtensa-lx106-elf-gcc

ifeq ($(COMPILE), xcc)
    AR = xt-ar
	CC = xt-xcc
	NM = xt-nm
	CPP = xt-xt++
	OBJCOPY = xt-objcopy
	OBJDUMP = xt-objdump
else
	AR = xtensa-lx106-elf-ar
	CC = xtensa-lx106-elf-gcc
	NM = xtensa-lx106-elf-nm
	CPP = xtensa-lx106-elf-g++
	OBJCOPY = xtensa-lx106-elf-objcopy
	OBJDUMP = xtensa-lx106-elf-objdump
endif
########################### END #############################


# ?= 条件赋值：只有此变量在之前没有赋值的情况下才会对这个变量进行赋值，它是递归展开的 #
#############################################################
# 参数默认配置

# =none - 不使用boot
# =old - 使用老版本的boot_v1.1+
# =new - 使用新版本的boot_v1.2+
BOOT?=none

# =0 - 不使用远程升级FOTA
# =1 - 使用FOTA，生成user1.<flash_map>.<BOOT>.bin，可支持云端升级
# =2 - 使用FOTA，生成user2.<flash_map>.<BOOT>.bin，可支持云端升级
APP?=0

# =40 - 40MHz
# =26.7 - 26.7MHz
# =20 - 20MHz
# =80 - 80MHz
SPI_SPEED?=40

# =QIO - Quad read and write mode（4倍读写模式）
# =QOUT - Quad read mode（4倍读模式）
# =DIO - Dual read and write mode（2倍读写模式）
# =DOUT - Dual read mode（2倍读模式）
SPI_MODE?=QIO

# =0 - flash size 512KB，代码区对半划分为 256KB + 256KB
# =2 - flash size 1024KB，代码区对半划分为 512KB + 512KB
# =3 - flash size 2048KB，前 1024KB为代码区，对半划分为 512KB + 512KB
# =4 - flash size 4096KB，前 1024KB为代码区，对半划分为 512KB + 512KB
# =5 - flash size 2048KB，代码区对半划分为 1024KB + 1024KB \
（仅 sdk_v1.1.0 + boot 1.4 + flash download tool_v1.2 及之后的版本支持）
# =6 - flash size 4096KB，前 2048KB为代码区，对半划分为 1024KB + 1024KB \
（仅 sdk_v1.1.0 + boot 1.4 + flash download tool_v1.2 及之后的版本支持）
SPI_SIZE_MAP?=0
########################### END #############################


#############################################################
# 参数选择

##### BOOT #####
ifeq ($(BOOT), new)
    boot = new
else
    ifeq ($(BOOT), old)
        boot = old
    else
        boot = none
    endif
endif

##### APP #####
ifeq ($(APP), 1)
    app = 1
else
    ifeq ($(APP), 2)
        app = 2
    else
        app = 0
    endif
endif

##### SPI_SPEED #####
ifeq ($(SPI_SPEED), 26.7)
    freqdiv = 1
else
    ifeq ($(SPI_SPEED), 20)
        freqdiv = 2
    else
        ifeq ($(SPI_SPEED), 80)
            freqdiv = 15
        else
            freqdiv = 0
        endif
    endif
endif

##### SPI_MODE #####
ifeq ($(SPI_MODE), QOUT)
    mode = 1
else
    ifeq ($(SPI_MODE), DIO)
        mode = 2
    else
        ifeq ($(SPI_MODE), DOUT)
            mode = 3
        else
            mode = 0
        endif
    endif
endif

# 初始化 user1.bin地址
addr = 0x01000

##### SPI_SIZE_MAP #####
ifeq ($(SPI_SIZE_MAP), 1)
  size_map = 1
  flash = 256
else
  ifeq ($(SPI_SIZE_MAP), 2)
    size_map = 2
    flash = 1024
    ifeq ($(app), 2)
      addr = 0x81000
    endif
  else
    ifeq ($(SPI_SIZE_MAP), 3)
      size_map = 3
      flash = 2048
      ifeq ($(app), 2)
        addr = 0x81000
      endif
    else
      ifeq ($(SPI_SIZE_MAP), 4)
        size_map = 4
        flash = 4096
        ifeq ($(app), 2)
          addr = 0x81000
        endif
      else
        ifeq ($(SPI_SIZE_MAP), 5)
          size_map = 5
          flash = 2048
          ifeq ($(app), 2)
            addr = 0x101000
          endif
        else
          ifeq ($(SPI_SIZE_MAP), 6)
            size_map = 6
            flash = 4096
            ifeq ($(app), 2)
              addr = 0x101000
            endif
          else
            size_map = 0
            flash = 512
            ifeq ($(app), 2)
              addr = 0x41000
            endif
          endif
        endif
      endif
    endif
  endif
endif
########################### END #############################


#############################################################
# 选择链接工具
# 这里选择了主目录下的 ld/eagle.app.v6.ld

LD_FILE = $(LDDIR)/eagle.app.v6.ld

# 如果boot!=none，才进入这里
ifneq ($(boot), none)
  ifneq ($(app),0)
    ifeq ($(size_map), 6)
      LD_FILE = $(LDDIR)/eagle.app.v6.$(boot).2048.ld
    else
      ifeq ($(size_map), 5)
        LD_FILE = $(LDDIR)/eagle.app.v6.$(boot).2048.ld
      else
        ifeq ($(size_map), 4)
          LD_FILE = $(LDDIR)/eagle.app.v6.$(boot).1024.app$(app).ld
        else
          ifeq ($(size_map), 3)
            LD_FILE = $(LDDIR)/eagle.app.v6.$(boot).1024.app$(app).ld
          else
            ifeq ($(size_map), 2)
              LD_FILE = $(LDDIR)/eagle.app.v6.$(boot).1024.app$(app).ld
            else
              ifeq ($(size_map), 0)
                LD_FILE = $(LDDIR)/eagle.app.v6.$(boot).512.app$(app).ld
              endif
            endif
          endif
        endif
      endif
    endif
  BIN_NAME = user$(app).$(flash).$(boot).$(size_map)
  endif
else
  app = 0
endif
########################### END #############################


# $(wildcard <pattern>)这个意思是获取工作目录下的所有的 pattern文件列表 #
# $(patsubst <pattern>,<replacement>,<text>)查找 <text> 中的单词 \
     （单词以“空格”、“Tab”或“回车”“换行”分隔）是否符合模式 <pattern> ，\
     如果匹配的话，则以 <replacement> 替换 #
#############################################################
# 用于查寻整个目录下的.c .cpp等文件以及 Makefile文件并将 Makefile路径后的 /替换为空

CSRCS ?= $(wildcard *.c)
CPPSRCS ?= $(wildcard *.cpp)
ASRCs ?= $(wildcard *.s)
ASRCS ?= $(wildcard *.S)
SUBDIRS ?= $(patsubst %/,%,$(dir $(wildcard */Makefile)))
########################### END #############################


#############################################################
# 定义各个输出目录的路径，obj,bin,lib,image....

ODIR := .output
OBJODIR := $(ODIR)/$(TARGET)/$(FLAVOR)/obj

# 设置 OBJS 变量
OBJS := $(CSRCS:%.c=$(OBJODIR)/%.o) \
        $(CPPSRCS:%.cpp=$(OBJODIR)/%.o) \
        $(ASRCs:%.s=$(OBJODIR)/%.o) \
        $(ASRCS:%.S=$(OBJODIR)/%.o)

# 设置 DEPS 变量
DEPS := $(CSRCS:%.c=$(OBJODIR)/%.d) \
        $(CPPSRCS:%.cpp=$(OBJODIR)/%.d) \
        $(ASRCs:%.s=$(OBJODIR)/%.d) \
        $(ASRCS:%.S=$(OBJODIR)/%.d)

# 设置 LIBODIR 库文件夹
LIBODIR := $(ODIR)/$(TARGET)/$(FLAVOR)/lib
OLIBS := $(GEN_LIBS:%=$(LIBODIR)/%)

# 设置 IMAGE 镜像文件
IMAGEODIR := $(ODIR)/$(TARGET)/$(FLAVOR)/image
OIMAGES := $(GEN_IMAGES:%=$(IMAGEODIR)/%)

# 设置 BINODIR 二进制文件夹
BINODIR := $(ODIR)/$(TARGET)/$(FLAVOR)/bin
OBINS := $(GEN_BINS:%=$(BINODIR)/%)
########################### END #############################


#############################################################
# 设置 CFLAGS和 DFLAGS变量

CCFLAGS += 			\
	-g			\
	-Wpointer-arith		\
	-Wundef			\
	-Werror			\
	-Wl,-EL			\
	-fno-inline-functions	\
	-nostdlib       	\
	-mlongcalls		\
	-mtext-section-literals \
	-ffunction-sections 	\
	-fdata-sections		\
	-fno-builtin-printf
#	-Wall			

CFLAGS = $(CCFLAGS) $(DEFINES) $(EXTRA_CCFLAGS) $(INCLUDES)
DFLAGS = $(CCFLAGS) $(DDEFINES) $(EXTRA_CCFLAGS) $(INCLUDES)
########################### END #############################


# $(foreach <var>,<list>,<text>)把参数 <list> 中的单词逐一取出 \
     放到参数 <var> 所指定的变量中， 然后再执行 <text> 所包含的表达式。\
     每一次 <text> 会返回一个字符串，循环过程中， <text> 的所返回的每个 \
     字符串会以空格分隔，最后当整个循环结束时， <text> 所返回的每个字符串 \
     所组成的整个字符串（以空格分隔）将会是foreach函数的返回值 #
# $(filter <pattern...>,<text>)以 <pattern> 模式过滤 <text> 字符 \
     串中的单词，保留符合模式 <pattern> 的单词。可以有多个模式 #
# $(dir <names...>)从文件名序列 <names> 中取出目录部分。目录部分是 \
     指最后一个反斜杠（ / ）之前 的部分。如果没有反斜杠，那么返回 ./ #
# $(if <condition>,<then-part>)或 $(if <condition>,<then-part>,<else-part>) \
  <condition> 参数是if的表达式，如果其返回的为非空字符串，那么这个 \
     表达式就相当于返回真， 于是， <then-part> 会被计算，否则 <else-part> 会被计算 #
#############################################################
# Functions
#

define ShortcutRule
$(1): .subdirs $(2)/$(1)
endef

define MakeLibrary
DEP_LIBS_$(1) = $$(foreach lib,$$(filter %.a,$$(COMPONENTS_$(1))),$$(dir $$(lib))$$(LIBODIR)/$$(notdir $$(lib)))
DEP_OBJS_$(1) = $$(foreach obj,$$(filter %.o,$$(COMPONENTS_$(1))),$$(dir $$(obj))$$(OBJODIR)/$$(notdir $$(obj)))
$$(LIBODIR)/$(1).a: $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1)) $$(DEPENDS_$(1))
	@mkdir -p $$(LIBODIR)
	$$(if $$(filter %.a,$$?),mkdir -p $$(EXTRACT_DIR)_$(1))
	$$(if $$(filter %.a,$$?),cd $$(EXTRACT_DIR)_$(1); $$(foreach lib,$$(filter %.a,$$?),$$(AR) xo $$(UP_EXTRACT_DIR)/$$(lib);))
	$$(AR) ru $$@ $$(filter %.o,$$?) $$(if $$(filter %.a,$$?),$$(EXTRACT_DIR)_$(1)/*.o)
	$$(if $$(filter %.a,$$?),$$(RM) -r $$(EXTRACT_DIR)_$(1))
endef

define MakeImage
DEP_LIBS_$(1) = $$(foreach lib,$$(filter %.a,$$(COMPONENTS_$(1))),$$(dir $$(lib))$$(LIBODIR)/$$(notdir $$(lib)))
DEP_OBJS_$(1) = $$(foreach obj,$$(filter %.o,$$(COMPONENTS_$(1))),$$(dir $$(obj))$$(OBJODIR)/$$(notdir $$(obj)))
$$(IMAGEODIR)/$(1).out: $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1)) $$(DEPENDS_$(1))
	@mkdir -p $$(IMAGEODIR)
	$$(CC) $$(LDFLAGS) $$(if $$(LINKFLAGS_$(1)),$$(LINKFLAGS_$(1)),$$(LINKFLAGS_DEFAULT) $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1))) -o $$@ 
endef

$(BINODIR)/%.bin: $(IMAGEODIR)/%.out
	@mkdir -p $(BIN_PATH)
	@mkdir -p $(BINODIR)
	
ifeq ($(APP), 0)
	@$(RM) -r $(BIN_PATH)/eagle.S $(BIN_PATH)/eagle.dump
	@$(OBJDUMP) -x -s $< > $(BIN_PATH)/eagle.dump
	@$(OBJDUMP) -S $< > $(BIN_PATH)/eagle.S
else
	@mkdir -p $(BIN_PATH)/upgrade
	@$(RM) -r $(BIN_PATH)/upgrade/$(BIN_NAME).S $(BIN_PATH)/upgrade/$(BIN_NAME).dump
	@$(OBJDUMP) -x -s $< > $(BIN_PATH)/upgrade/$(BIN_NAME).dump
	@$(OBJDUMP) -S $< > $(BIN_PATH)/upgrade/$(BIN_NAME).S
endif

	@$(OBJCOPY) --only-section .text -O binary $< eagle.app.v6.text.bin
	@$(OBJCOPY) --only-section .data -O binary $< eagle.app.v6.data.bin
	@$(OBJCOPY) --only-section .rodata -O binary $< eagle.app.v6.rodata.bin
	@$(OBJCOPY) --only-section .irom0.text -O binary $< eagle.app.v6.irom0text.bin
########################### END #############################


#############################################################
# 下面是输出打印信息	

	@echo ""
	@echo "!!!"
	@echo "SDK_PATH: $(SDK_PATH)"
	
ifeq ($(app), 0)
	@python $(SDK_PATH)/tools/gen_appbin.py $< 0 $(mode) $(freqdiv) $(size_map)
	@mv eagle.app.flash.bin $(BIN_PATH)/eagle.flash.bin
	@mv eagle.app.v6.irom0text.bin $(BIN_PATH)/eagle.irom0text.bin
	@rm eagle.app.v6.*
	@echo "BIN_PATH: $(BIN_PATH)"
	@echo ""
	@echo "No boot needed."
	@echo "Generate eagle.flash.bin and eagle.irom0text.bin successully in BIN_PATH"
	@echo "eagle.flash.bin-------->0x00000"
	@echo "eagle.irom0text.bin---->0x20000"
else
	@echo "BIN_PATH: $(BIN_PATH)/upgrade"
	@echo ""

    ifneq ($(boot), new)
		@python $(SDK_PATH)/tools/gen_appbin.py $< 1 $(mode) $(freqdiv) $(size_map)
		@echo "Support boot_v1.1 and +"
    else
		@python $(SDK_PATH)/tools/gen_appbin.py $< 2 $(mode) $(freqdiv) $(size_map)

    	ifeq ($(size_map), 6)
		@echo "Support boot_v1.4 and +"
        else
            ifeq ($(size_map), 5)
		@echo "Support boot_v1.4 and +"
            else
		@echo "Support boot_v1.2 and +"
            endif
        endif
    endif

	@mv eagle.app.flash.bin $(BIN_PATH)/upgrade/$(BIN_NAME).bin
	@rm eagle.app.v6.*
	@echo "Generate $(BIN_NAME).bin successully in BIN_PATH"
	@echo "boot.bin------------>0x00000"
	@echo "$(BIN_NAME).bin--->$(addr)"
endif

	@echo "!!!"
########################### END #############################


#############################################################
# Rules base
# Should be done in top-level makefile only
#

# 用一个特殊的标记“.PHONY”来显式地指明一个目标是“ 伪目标” #
#############################################################
# make执行

# make all执行的方法(默认目标)
all:	.subdirs $(OBJS) $(OLIBS) $(OIMAGES) $(OBINS) $(SPECIAL_MKTARGETS)

# make clean执行的方法
.PHONY: clean
clean:
	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d) clean;)
	$(RM) -r $(ODIR)/$(TARGET)/$(FLAVOR)

# 
clobber: $(SPECIAL_CLOBBER)
	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d) clobber;)
	$(RM) -r $(ODIR)

.subdirs:
	@set -e; $(foreach d, $(SUBDIRS), $(MAKE) -C $(d);)

#.subdirs:
#	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d))


#############################################################
# 这段代码比较典型，MAKECMDGOALS并没有定义，\
# 所以会执行sinclude $(DEPS)，\
# DEPS根据前面的定义可以知道是当前目录下的源文件(.c .cpp .s)生成的.d，\
# 根据.d文件的生成规则可以知道是使用gcc -M 编译得到，\
# 即对应.o的依赖关系，包括包含的.h(新建a.c文件仅写一个a.h，\
# 新建a.h放空，编译后的a.d为.output/eagle/debug/obj/a.o \
# .output/eagle/debug/obj/a.d : a.c a.h)，\
# sinclude $(DEPS)就是将这个.d文件展开，\
# 意义在于我们写依赖关系的时候我们并不能把源文件里面引用的.h文件都加到依赖关系里面，\
# 如果不加进来，那么仅修改.h的不会重新生成.o文件的。\
# 简单一句话就是.c文件中包含的.h发生改变的时候重新生成对应的.o

ifneq ($(MAKECMDGOALS),clean)
  ifneq ($(MAKECMDGOALS),clobber)
    ifdef DEPS
      sinclude $(DEPS)
    endif
  endif
endif
########################### END #############################


# “$@” 表示规则中的目标文件集
# “$<” 表示依赖目标中的第一个目标名字
# “$?” 所有比目标新的依赖目标的集合。以空格分隔
# “$?” 所有比目标新的依赖目标的集合。以空格分隔
# “$^” 所有的依赖目标的集合
# $(findstring <find>,<in>)在字串 <in> 中查找 <find> 字串
# $(eval <text>) text 的内容将作为makefile的一部分而被make解析和执行
#############################################################
# 生成输出目录下的同名文件

$(OBJODIR)/%.o: %.c
	@mkdir -p $(OBJODIR);
	$(CC) $(if $(findstring $<,$(DSRCS)),$(DFLAGS),$(CFLAGS)) $(COPTS_$(*F)) -o $@ -c $<

$(OBJODIR)/%.d: %.c
	@mkdir -p $(OBJODIR);
	@echo DEPEND: $(CC) -M $(CFLAGS) $<
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	
$(OBJODIR)/%.o: %.cpp
	@mkdir -p $(OBJODIR);
	$(CPP) $(if $(findstring $<,$(DSRCS)),$(DFLAGS),$(CFLAGS)) $(COPTS_$(*F)) -o $@ -c $<

$(OBJODIR)/%.d: %.cpp
	@mkdir -p $(OBJODIR);
	@echo DEPEND: $(CPP) -M $(CFLAGS) $<
	@set -e; rm -f $@; \
	$(CPP) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJODIR)/%.o: %.s
	@mkdir -p $(OBJODIR);
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJODIR)/%.d: %.s
	@mkdir -p $(OBJODIR); \
	set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJODIR)/%.o: %.S
	@mkdir -p $(OBJODIR);
	$(CC) $(CFLAGS) -D__ASSEMBLER__ -o $@ -c $<

$(OBJODIR)/%.d: %.S
	@mkdir -p $(OBJODIR); \
	set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(foreach lib,$(GEN_LIBS),$(eval $(call ShortcutRule,$(lib),$(LIBODIR))))

$(foreach image,$(GEN_IMAGES),$(eval $(call ShortcutRule,$(image),$(IMAGEODIR))))

$(foreach bin,$(GEN_BINS),$(eval $(call ShortcutRule,$(bin),$(BINODIR))))

$(foreach lib,$(GEN_LIBS),$(eval $(call MakeLibrary,$(basename $(lib)))))

$(foreach image,$(GEN_IMAGES),$(eval $(call MakeImage,$(basename $(image)))))
########################### END #############################


#############################################################
# Recursion Magic - Don't touch this!!
#
# Each subtree potentially has an include directory
#   corresponding to the common APIs applicable to modules
#   rooted at that subtree. Accordingly, the INCLUDE PATH
#   of a module can only contain the include directories up
#   its parent path, and not its siblings
#
# Required for each makefile to inherit from the parent
#


# += 追加赋值：给变量追加内容，但会在追加内容之前添加一个空格。\
     它是直接展开还是递归展开取决于该变量上一次赋值的方式。
#############################################################
# 设置头文件路径
# 把主目录下的 include头文件包含进来

INCLUDES := $(INCLUDES) -I $(SDK_PATH)/include -I $(SDK_PATH)/extra_include
#INCLUDES += -I $(SDK_PATH)/driver_lib/include
INCLUDES += -I $(SDK_PATH)/app/include
INCLUDES += -I $(SDK_PATH)/include/espressif
INCLUDES += -I $(SDK_PATH)/include/lwip
INCLUDES += -I $(SDK_PATH)/include/lwip/ipv4
INCLUDES += -I $(SDK_PATH)/include/lwip/ipv6
INCLUDES += -I $(SDK_PATH)/include/nopoll
INCLUDES += -I $(SDK_PATH)/include/spiffs
INCLUDES += -I $(SDK_PATH)/include/ssl
INCLUDES += -I $(SDK_PATH)/include/json
########################### END #############################

