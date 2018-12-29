#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
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
	{ 0x7e1e80df, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xf65aed07, "kmalloc_caches" },
	{ 0xd2b09ce5, "__kmalloc" },
	{ 0xabd0c91c, "rtc_time_to_tm" },
	{ 0xadaabe1b, "pv_lock_ops" },
	{ 0x349cba85, "strchr" },
	{ 0x20eadeb6, "ip_compute_csum" },
	{ 0x754d539c, "strlen" },
	{ 0x34184afe, "current_kernel_time" },
	{ 0xe6a654cc, "__register_chrdev" },
	{ 0xad66d3dc, "nf_register_hook" },
	{ 0x91715312, "sprintf" },
	{ 0xe2d5255a, "strcmp" },
	{ 0x4f8b5ddb, "_copy_to_user" },
	{ 0xfb578fc5, "memset" },
	{ 0x243f4fa0, "skb_checksum" },
	{ 0x27e1a049, "printk" },
	{ 0x20c55ae0, "sscanf" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0xb4390f9a, "mcount" },
	{ 0x1e6d26a8, "strstr" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x940f194b, "kmem_cache_alloc_trace" },
	{ 0xd52bf1ce, "_raw_spin_lock" },
	{ 0x4f68e5c9, "do_gettimeofday" },
	{ 0x21c74976, "nf_unregister_hook" },
	{ 0x37a0cba, "kfree" },
	{ 0x69acdf38, "memcpy" },
	{ 0xe113bbbc, "csum_partial" },
	{ 0x4f6b400b, "_copy_from_user" },
	{ 0xe914e41e, "strcpy" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "722B5BBF5B348F43A034A7C");
