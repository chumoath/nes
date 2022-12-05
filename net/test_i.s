	.file	"test_i.cpp"

	.text
	.globl	b
	
	.data
	.align 4
	.type	b, @object
	.size	b, 4

# 全局数据
b:
	.long	5


	.section	.text._ZN1AC2Eii,"axG",@progbits,_ZN1AC5Eii,comdat
	.align 2
	.weak	_ZN1AC2Eii
	.type	_ZN1AC2Eii, @function
_ZN1AC2Eii:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %edx
	movl	%edx, (%rax)
	movq	-8(%rbp), %rax
	movl	-16(%rbp), %edx
	movl	%edx, 4(%rax)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	_ZN1AC2Eii, .-_ZN1AC2Eii
	.weak	_ZN1AC1Eii
	.set	_ZN1AC1Eii,_ZN1AC2Eii
	.globl	a
	
	
	.bss
	.align 8
	.type	a, @object
	.size	a, 8

# 全局对象
a:
	.zero	8


	.text
	.globl	main
	.type	main, @function
main:
.LFB3:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	main, .-main
	.type	_Z41__static_initialization_and_destruction_0ii, @function
	
_Z41__static_initialization_and_destruction_0ii:
.LFB4:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$1, -4(%rbp)
	jne	.L6
	cmpl	$65535, -8(%rbp)
	jne	.L6
	movl	$2, %edx
	movl	$1, %esi
	leaq	a(%rip), %rax
	movq	%rax, %rdi
	call	_ZN1AC1Eii
.L6:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	_Z41__static_initialization_and_destruction_0ii, .-_Z41__static_initialization_and_destruction_0ii
	.type	_GLOBAL__sub_I_b, @function
_GLOBAL__sub_I_b:
.LFB5:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$65535, %esi
	movl	$1, %edi
	call	_Z41__static_initialization_and_destruction_0ii
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	_GLOBAL__sub_I_b, .-_GLOBAL__sub_I_b
	.section	.init_array,"aw"
	.align 8
	.quad	_GLOBAL__sub_I_b
	.ident	"GCC: (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
