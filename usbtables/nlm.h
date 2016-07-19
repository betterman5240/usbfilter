/*
 * nlm.h
 * Header file for netlink messaging
 * Most things come from drivers/usb/filter/usb_filter.h
 * Jul 23, 2015
 * root@davejingtian.org
 * http://davejingtian.org
 *
 */

#ifndef NLM_INCLUDE
#define NLM_INCLUDE

#include <sys/types.h>

#define USBFILTER_TASK_COMM_LEN			16
#define USBFILTER_DEV_TAB_DEVPATH_LEN		16
#define USBFILTER_DEV_TAB_PRODUCT_LEN		32
#define USBFILTER_DEV_TAB_MANUFACTURER_LEN	32
#define USBFILTER_DEV_TAB_SERIAL_LEN		32
#define USBFILTER_PKT_TAB_TYPE_ISO		0
#define USBFILTER_PKT_TAB_TYPE_INT		1	/* The 4 types should be lined up with the PIPE types */
#define USBFILTER_PKT_TAB_TYPE_CTRL		2
#define USBFILTER_PKT_TAB_TYPE_BULK		3
#define USBFILTER_PKT_TAB_DIR_OUT		0	/* host 2 device */
#define USBFILTER_PKT_TAB_DIR_IN		1	/* device 2 host */
#define USBFILTER_LUM_TAB_NAME_LEN		32
#define USBFILTER_TAB_VALID			1
#define USBFILTER_RULE_NAME_LEN			32
#define USBFILTER_RULE_ACTION_ALLOW		0
#define USBFILTER_RULE_ACTION_DROP		1
#define USBFILTER_NETLINK			31
#define USBFILTER_NETLINK_OPC_INIT		0	/* init the netlink socket */
#define USBFILTER_NETLINK_OPC_ADD		1	/* add a new rule */
#define USBFILTER_NETLINK_OPC_DEL		2	/* delete a rule */
#define USBFILTER_NETLINK_OPC_SYN		3	/* sync rules from the user space */
#define USBFILTER_NETLINK_OPC_ENA		4	/* enable usbfilter */
#define USBFILTER_NETLINK_OPC_DIS		5	/* disable usbfilter */
#define USBFILTER_NETLINK_OPC_CHG		6	/* change the default behavior of usbfilter */
#define USBFILTER_NETLINK_OPC_DMP		7	/* dump the kernel simple rules */
#define USBFILTER_NETLINK_OPC_ACK		8	/* acknowledgement used by the kernel */
#define USBFILTER_NETLINK_RES_SUCCESS		0
#define USBFILTER_NETLINK_RES_FAILURE		(-1)
#define USBFILTER_NETLINK_RES_LAST		1
#define USBFILTER_TYPE_RULE			0
#define USBFILTER_TYPE_SIM_RULE			1
#define USBFILTER_SIM_RULE_TYPE_PGID		0
#define USBFILTER_SIM_RULE_TYPE_COMM		1
#define USBFILTER_SIM_RULE_ENTRY_NUM		16
#define USBFILTER_CONFIG_DMP_NAME		"config_dmp"
#define USBFILTER_MBM_SEC_IN_USEC		1000000	/* micro benchmark */
#define USBFILTER_MBM_SUB_TV(s,e)		\
	((e.tv_sec*USBFILTER_MBM_SEC_IN_USEC+e.tv_usec) - \
	(s.tv_sec*USBFILTER_MBM_SEC_IN_USEC+s.tv_usec))

/* process table */
struct proc_tab {
	int	valid;			/* shortcut to parse the table */
	pid_t	pid;
	pid_t	ppid;
	pid_t	pgid;
	uid_t	uid;
	uid_t	euid;
	gid_t	gid;
	gid_t	egid;
	char	comm[USBFILTER_TASK_COMM_LEN];	/* 16 bytes */
};

/* simple pgid table */
struct sim_pgid_tab {
	pid_t	pgid[USBFILTER_SIM_RULE_ENTRY_NUM];
};

/* simple comm table */
struct sim_comm_tab {
	char	comm[USBFILTER_SIM_RULE_ENTRY_NUM][USBFILTER_TASK_COMM_LEN];
};

/* device table */
struct dev_tab {
	int	valid;
	int	busnum;
	int	devnum;
	int	portnum;
	int	ifnum;
	char 	devpath[USBFILTER_DEV_TAB_DEVPATH_LEN];
	char	product[USBFILTER_DEV_TAB_PRODUCT_LEN];
	char	manufacturer[USBFILTER_DEV_TAB_MANUFACTURER_LEN];
	char	serial[USBFILTER_DEV_TAB_SERIAL_LEN];
};

/* packet table
 * These are actually from 'pipe'
 */
struct pkt_tab {
	int	valid;
	int	type;
	int	direction;
	int	endpoint;
	int	address;
};

/* lum table */
struct lum_tab {
	int 	valid;
	char	name[USBFILTER_LUM_TAB_NAME_LEN];
};

/* rule */
struct usbfilter_rule {
	int	action;
	char	name[USBFILTER_RULE_NAME_LEN];
	struct proc_tab	proc;
	struct dev_tab	dev;
	struct pkt_tab	pkt;
	struct lum_tab	lum;
	void	*mod;	/* pointing to the real lum struct used by the kernel */
};

/* simple rule mainly used by kernel activities whitelist */
struct usbfilter_sim_rule {
	int	action;
	char	name[USBFILTER_RULE_NAME_LEN];
	int	type;
	union {
		struct sim_pgid_tab pgid_tab;
		struct sim_comm_tab comm_tab;
	};
};

/* netlink msg */
struct usbfilter_nlmsg {
	int	opcode;
	int	type;
	int	behavior;
	int	result;		/* used by ACK */
	union {
		struct usbfilter_rule		rule;
		struct usbfilter_sim_rule	sim_rule;
	};
};


/* Definition for the netlink msgs */
typedef struct usbfilter_nlmsg nlmsgt;

#define NLM_UCHAR_NUM_PER_LINE	20
#define NLM_MSG_LEN		sizeof(nlmsgt)
#define NLM_QUEUE_MSG_NUM	10
#define NLM_QUEUE_SIZE		(NLM_MSG_LEN*NLM_QUEUE_MSG_NUM)

/* NLM protocol related methods */

/* Display the nlmsgt msg */
void nlm_display_msg(nlmsgt *msg);

/* Display the uchar given length */
void nlm_display_uchar(unsigned char *src, int len, char *header);

/* NLM queue related methods */

/* Init the NLM queue */
void nlm_init_queue(void);

/* Add msgs into the NLM queue from raw binary data */
int nlm_add_msg_queue(nlmsgt *msg);

/* Clear all the msgs in the queue */
void nlm_clear_all_msg_queue(void);

/* Get the number of msgs in the queue */
int nlm_get_msg_num_queue(void);

/* Get the msg based on index */
nlmsgt * nlm_get_msg_queue(int index);

#endif
