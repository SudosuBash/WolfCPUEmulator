# 枚举 MMIO 设备的基地址

[baseAddr = 0xffff0000] # 此程序被加载到0xffff0000地址处执行

# BUS:
# 当前枚举设备 IN 0xfffffd00 - 2Bytes
# 执行命令 0xfffffd02 - 1Byte
# 写入命令结果 0xfffffd04 - 4Bytes
# 读取该设备需要的空间 0xfffffd08 - 2Bytes
# 当前设备vendor_id OUT 0xfffffd0a - 2Bytes

# CMD 2: 设置基址 val=基地址
# CMD 3: 设置中断号 val=中断号

jmp main_bios


str_title:
    dbstr "Wolf CPU Emulator BIOS Version 1.0.\n",0
str_error_timeout:
    dbstr "Error: Device Request Timeout.\n",0
str_error_illegal_instruction:
    dbstr "Error: Illegal Instruction.\n",0
timeout:
    movwh r12,0xffff
    movwl r12,str_error_timeout
    call output_str
    jmp fffc # 死机


# 输入:
# r1 = 执行指令
# r10 = 基地址
# r9 = 中断号
# r11 = need_space
# r13 = 执行地址
# r12 = vendor_id
# ========= MMIO_SET START =========
set_base_addr:
    movwl r13,0xfd04 #写入命令寄存器
    movd [r13],r10   #写入命令
    movwl r13,0xfd02
    movb r1,2 #执行设置基址命令
    movb [r13],r1    #执行命令
    add r10,r11    #基地址+need_space
    ret

set_irqnum:
    movwl r13,0xfd04 #写入命令寄存器
    movd [r13],r9   #写入命令
    movwl r13,0xfd02
    movb r1,3 #执行设置中断号命令
    movb [r13],r1    #执行命令
    add r9,1    #中断号+1
    ret

# r8 = 设备树地址
# 定义设备树地址 0x00000000
# 设备树结构:
#   vendor ID: 2 bytes
#   base address: 4 bytes
#   irq number: 2 bytes
#   need space: 2 bytes


store_to_device_tree:
    movwl [r8],r12  #存储vendor_id
    add r8,2
    movd [r8],r10  #存储base_address
    add r8,4
    movwl [r8],r9   #存储irq_number
    add r8,2
    movwl [r8],r11  #存储need_space
    add r8,2
    ret

mmio_set:
    xor r10,r10 #基地址
    xor r9,r9 #中断号
    movwh r10,0xffff
    movwh r13,0xffff

    movb r1,0
    movwl r13,0xfd00
    movwl [r13],r1

mmio_set_start:
    movwl r8,0xfffc
    movwh r13,0xfd0a
    movwl r12,[r13]  #vendor_id入r12

    xorn r12,r12 #判断vendor_id是否为0
    jne mmio_set_end #不为0继续枚举

    movwl r13,0xfd08 #读取need_space
    movwl r11,[r13] #need_space入r11

    # 用一个特殊的寄存器暂时存储输出的base_address,就r2吧
    # 输出的vendor_id = 0x1002
    subn r12,0x1002
    jne mmio_set_continue
    movd r2,r11  # r2 = base_address
mmio_set_continue:
    call set_base_addr #调用MMU控制器的设置基址函数
    call set_irqnum  #调用MMU控制器的设置中断号函数    
    call store_to_device_tree #存储到设备树
    jmp mmio_set_start
mmio_set_end:
    ret
# ========= MMIO_SET END =========

# r13 = string segment offset
# r14 = output device base address
# 输出设备地址定义:
# OFFSET = 0x2: 输出字符
# OFFSET = 0x0: 输出命令
# OFFSET = 0x1: 状态(通常不用)
output_str:
    push r1 #保存r1
output_main:
    movb r1,[r13] #读取字符
    xorn r1,r1 #判断是否结束
    je output_end #结束
    
    add r14,2 #移动到输出寄存器
    movb [r14],r1 
    sub r14,2 #移动到命令寄存器
    movb r1,0x1
    movb [r14],r1 #输送命令,输出字符

    add r14,2
    add r13,1 #下一个字符
output_end:
    pop r1 #恢复r1
    ret

irq_register:
    mov offset r1,ecall_handler_num1
    lebr r1 # 中断向量表基址
    ret

main_bios:
    call mmio_set #枚举 MMIO 设备
    movd r14,r2 # r14 = 暂时存储输出的base_address

    movwh r12,0xffff
    movwl r12,str_title
    #标题字符串地址
    call output_str #输出标题字符串
    
    ret

ecall_handler_machine_problem:
    rere r1 # 加载异常原因

ecall_handler_num1:
    mov offset r1,ecall_handler_machine_problem 
    #伪指令，实际上等于两条条指令，会集成到汇编器
    #movwh r1,计算出来地址低16位
    #movwl r1,计算出来地址高16位
    jmp r1 //跳转到处理函数