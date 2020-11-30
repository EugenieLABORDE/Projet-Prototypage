**Membres du groupe: **
Eugénie Laborde
Emilie Laganier 
Paul Martinez
Antoine Reboullet

**Guide utilisation Servomoteur sur ROS dans la VM: **

- Lancer la VM 

- Réaliser le branchement du servomoteur au PC 

- Aller dans Périphériques de la VM et cliquer sur Xevelabs afin de relier le servomoteur à la VM 

- Regarder le nom exact du périphérique avec la commande “ls /dev” (pour le servomoteur 3, il s’agira soit de ttyACM1 ou de ttyACM0) 

- Aller dans le dynamixel workspace (initialement créé) “cd Bureau/dynamixel_ws/” 

- Effectuer la commande “source devel/setup.bash” 

- Effectuer la commande : “rosrun dynamixel_driver info_dump.py -p /dev/ttyACM0 3” (si erreur, changer le 3) pour vérifier la communication avec le moteur. Si la permission est refusée, alors effectuer la commande “sudo chmod 666 /dev/ttyACM0” 

- Lancer le roscore 

- Lancer le contrôleur manageur : “roslaunch dynamixel_tutorials controller_manager.launch” (s'il y a une erreur modifier le fichier en vérifiant que le port soit bien le bon). Il est possible de vérifier que cette commande a fonctionné en effectuant rostopic list et echo comme montré dans ce tutoriel : http://wiki.ros.org/dynamixel_controllers/Tutorials/ConnectingToDynamixelBus 

- Effectuer dans un nouveau terminal la commande : roslaunch dynamixel_tutorials controller_spawner.launch 

Commander le Servomoteur par le terminal linux : 

“rostopic pub -1 /pan_controller/command std_msgs/Float64 -- POSITION”. La position doit être un float compris entre –3 et 3 (0 est la position initiale). 


Commander Servomoteur par l’IDE Arduino: 

- Lancer Arduino IDE en allant dans /home/student/arduino-1.8.13 et lancer “./arduino” 

- Connecter l’ESP32 à la VM (QinHeng...) 

- Toujours dans la même bibliothèque, effectuer la commande suivante : “rosrun rosserial_arduino serial_node.py” qui permet d’établir la connexion PC / ESP32 au travers de la liaison série 

**Lien vers la vidéo: ** 
https://www.youtube.com/watch?v=xan8CuXWHUY&feature=youtu.be&fbclid=IwAR3ySiJ_d23HNKAn7BspxO3mUu0GQLXsomEc5avmeNnZE2Y70LthB8SJTtQ

