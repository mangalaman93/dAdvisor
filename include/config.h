// various constants and parameters

// type of object
#define TYPE_CONTAINER "container"
#define TYPE_LPROCESS "lprocess"

// period before collect usage again
#define USAGE_CHECK_PERIOD 500000

// cgroup path
#define BASE_URL "/sys/fs/cgroup/"
#define CPUACCT_URL "cpuacct/"
#define CPU_URL "cpu/"
#define CPUSET_URL "cpuset/"
#define DOCKER_URL "docker/"
#define CPU_STAT_FILE "cpuacct.stat"
#define CPU_SHARES_FILE "cpu.shares"
#define CPUSET_FILE "cpuset.cpus"
#define CPU_CFS_QUOTA_FILE "cpu.cfs_quota_us"
#define CPU_CFS_PERIOD_FILE "cpu.cfs_period_us"

// monitoring parameters
// period of monitoring in sec
#define TRIGGER_PERIOD 2

// allocation safety factor
#define SAFETY_FACTOR 0.8

// increase allocation by a factor of 0.2
#define INCREMENT_FACTOR 1
