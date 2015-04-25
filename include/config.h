// type of object
#define TYPE_CONTAINER "container"
#define TYPE_LPROCESS "lprocess"
#define TYPE_CGROUP "cgroup"

// cgroup path
#define DEFAULT_HANDLE 10
#define DEFAULT_DOCKER_IFACE "eth0"
#define BASE_URL "/sys/fs/cgroup/"
#define DOCKER_URL "docker/"
#define CPUACCT_URL "cpuacct/"
#define CPU_URL "cpu/"
#define CPUSET_URL "cpuset/"
#define CPU_STAT_FILE "cpuacct.stat"
#define CPU_SHARES_FILE "cpu.shares"
#define CPUSET_FILE "cpuset.cpus"
#define CPU_CFS_QUOTA_FILE "cpu.cfs_quota_us"
#define CPU_CFS_PERIOD_FILE "cpu.cfs_period_us"
#define NET_CLASS "net_cls/"
#define TASKS_FILE "tasks"
#define CLASSID_FILE "net_cls.classid"
#define NET_FOLDER "/sys/class/net/"
#define NET_TX_FILE "/sys/devices/virtual/net/eth0/statistics/tx_bytes"
#define NET_RX_FILE "/sys/devices/virtual/net/eth0/statistics/rx_bytes"

/* ---------- control parameters ---------- */
// period before collect usage again
#define USAGE_CHECK_PERIOD 2

// allocation safety factor
#define SAFETY_FACTOR 0.8

// increase allocation by a factor of 0.2
#define INCREMENT_FACTOR 0.2
