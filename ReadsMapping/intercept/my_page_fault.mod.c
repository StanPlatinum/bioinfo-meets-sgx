#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xf5685d24, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xa16acb66, __VMLINUX_SYMBOL_STR(do_page_fault_handler) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xdc0872e8, __VMLINUX_SYMBOL_STR(enclave_size) },
	{ 0x88886e8f, __VMLINUX_SYMBOL_STR(default_do_page_fault) },
	{ 0x67ee5fbb, __VMLINUX_SYMBOL_STR(enclave_baseaddr) },
	{ 0x7a1e2f87, __VMLINUX_SYMBOL_STR(find_vma) },
	{ 0x5d861569, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x19f3ac66, __VMLINUX_SYMBOL_STR(pv_mmu_ops) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=isgx";


MODULE_INFO(srcversion, "EE12CEC9D6D94ED06F1B6E1");
