#############################################################
# Required variables for each makefile
# Discard this section from all parent makefiles
# Expected variables (with automatic defaults):
#   CSRCS (all "C" files in the dir)
#   SUBDIRS (all subdirs with a Makefile)
#   GEN_LIBS - list of libs to be generated ()
#   GEN_IMAGES - list of object file images to be generated ()
#   GEN_BINS - list of binaries to be generated ()
#   COMPONENTS_xxx - a list of libs/objs in the form
#     subdir/lib to be extracted and rolled up into
#     a generated lib/image xxx.a ()
#

TARGET = eagle

#############################################################
# 选择debug工程版本还是release

#FLAVOR = release
FLAVOR = debug
########################### END #############################


#############################################################
# 获取工程目录

parent_dir:=$(abspath $(shell pwd)/$(lastword $(MAKEFILE_LIST)))
parent_dir:=$(shell dirname $(parent_dir))
parent_dir:=$(shell dirname $(parent_dir))

# 工程主目录
SDK_PATH=$(parent_dir)

# bin目录
BIN_PATH=$(SDK_PATH)/bin

########################### END #############################


#EXTRA_CCFLAGS += -u


#############################################################
# 定义SDK_PATH和BIN_PATH两个目录(工程根目录和bin目录)

ifndef PDIR # {
GEN_IMAGES= eagle.app.v6.out
GEN_BINS= eagle.app.v6.bin
SPECIAL_MKTARGETS=$(APP_MKTARGETS)

# 子目录，如果你在app里新建了一个源文件目录，需要添加到这里
# 否则不会编译未添加的目录的源文件！
SUBDIRS=	\
	user	\
	driver  \
	user/bsp/driver	\
	sample_lib
	
# 比如添加了一个 network 文件夹，则：
# SUBDIRS=  \
#   user    \
#   driver  \
#   network

endif # } PDIR
########################### END #############################


# 定义一个主目录下的 ld文件的变量
LDDIR = $(SDK_PATH)/ld

CCFLAGS += -Os

TARGET_LDFLAGS =		\
	-nostdlib		\
	-Wl,-EL			\
	--longcalls		\
	--text-section-literals


#############################################################
# 根据 FLAVOR 变量选择优化等级
# -O0不优化，-O2中级优化

ifeq ($(FLAVOR),debug)
    TARGET_LDFLAGS += -g -O2
endif

ifeq ($(FLAVOR),release)
    TARGET_LDFLAGS += -g -O0
endif
########################### END #############################


#############################################################
# 添加静态链接库，对应前面 SUBDIRS 变量设置的目录
# 每一个文件夹里的源代码先编译成对应的静态库，然后再进行链接
# 所以如果添加新的源码文件夹，也需要在这里添加对应的静态库！

COMPONENTS_eagle.app.v6 =	\
	user/libuser.a		\
	driver/libdriver.a 	\
	user/bsp/driver/libbsp.a		\
	sample_lib/libsample.a

# 比如添加network：
# COMPONENTS_eagle.app.v6 =	\
#   user/libuser.a          \ # 链接库
#   driver/libdriver.a  	\ # ld文件
#   network/libnetwork.a
########################### END #############################


#############################################################
# 链接参数
# 参考lib文件夹里的静态链接库
# 比如，如果需要使用smartconfig接口
# 即，调用主目录 include的smartconfig.h文件
# 只需要添加 -lsmartconfig 即可。

LINKFLAGS_eagle.app.v6 =	\
	-L$(SDK_PATH)/lib	\
	-Wl,--gc-sections	\
	-nostdlib			\
	-T$(LD_FILE)		\
	-Wl,--no-check-sections	\
	-u call_user_start	\
	-Wl,-static		\
	-Wl,--start-group	\
	-lcirom			\
	-lcrypto		\
	-lespconn		\
	-lespnow		\
	-lfreertos		\
	-lgcc			\
	-lhal			\
	-ljson			\
	-llwip			\
	-lmain			\
	-lmesh			\
	-lmirom			\
	-lnet80211		\
	-lnopoll		\
	-lphy			\
	-lpp			\
	-lpwm			\
	-lsmartconfig	\
	-lspiffs		\
	-lssl			\
	-lwpa			\
	-lwps			\
	-lairkiss		\
	$(DEP_LIBS_eagle.app.v6)\
	-Wl,--end-group
	
# 比如，如果需要使用smartconfig接口
# 只需要添加 -lsmartconfig 即可。
# -Wl,--start-group前面的为链接参数和-Wl,--end-group间为链接库，可以根据需要进行删减。
# LINKFLAGS_eagle.app.v6 =    	\
# 	-L$(SDK_PATH)/lib			\ # 定义链接库的搜索路径是 SDK/lib
# 	-Wl,--gc-sections         	\ # 减少静态库不必要的调用
# 	-nostdlib                 	\ # 不使用标准库 
# 	-T$(LD_FILE)              	\ # 读取链接描述脚本，以确定符号等的定位地址
# 	-Wl,--no-check-sections   	\ # Do not check section addresses for overlaps  不检查重叠地址
# 	-u call_user_start        	\ # 取消定义的宏(call_user_start)
# 	-Wl,-static               	\ # 使用静态链接
# 	-Wl,--start-group         	\ # 库列表开始
#	。。。。。。。。				\ # 各种库接口
# 	-lsmartconfig				\ # 快速连接smartconfig
# 	-Wl,--end-group			  	# 库列表结束

DEPENDS_eagle.app.v6 =		\
                $(LD_FILE)	\
                $(LDDIR)/eagle.rom.addr.v6.ld
########################### END #############################                


#############################################################
# Configuration i.e. compile options etc.
# Target specific stuff (defines etc.) goes in here!
# Generally values applying to a tree are captured in the
#   makefile at its root level - these are then overridden
#   for a subtree within the makefile rooted therein
#

#UNIVERSAL_TARGET_DEFINES =		\

# Other potential configuration flags include:
#	-DTXRX_TXBUF_DEBUG
#	-DTXRX_RXBUF_DEBUG
#	-DWLAN_CONFIG_CCX
CONFIGURATION_DEFINES =	-DICACHE_FLASH

DEFINES +=				\
	$(UNIVERSAL_TARGET_DEFINES)	\
	$(CONFIGURATION_DEFINES)

DDEFINES +=				\
	$(UNIVERSAL_TARGET_DEFINES)	\
	$(CONFIGURATION_DEFINES)


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

# 把当前目录下的 include 等文件夹包含进来
INCLUDES := $(INCLUDES) -I $(PDIR)include
INCLUDES += -I $(PDIR)user/bsp/include

# 把子目录下的Makefile加进来
# sinclude：以忽略错误的方式执行
sinclude $(SDK_PATH)/Makefile

# 定义了一个FORCE的伪目标，啥都没做
.PHONY: FORCE
FORCE:
########################### END #############################

