# betaRosNode
ros node for publishing beta. It subscribes for two point cloud ("points_init", "points_def"). It publishes a Float32MultiArray as "beta". The first element of the array is beta, the second is the error, the third is id of received point cloud. 
