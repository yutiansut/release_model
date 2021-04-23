run2 vector => array 2800 => 2200 ns
run3 TreeDepth => 6 2200 => 1400ns
run4 TreeCount => define 没有改善
run5 int => int16_t, int8_t
run6 delete static 增加60ns的运行时间

summary:
三个有效的优化方法： 
1）TreeDepth 改成常量
2) vetor => 原生数组
3) static
