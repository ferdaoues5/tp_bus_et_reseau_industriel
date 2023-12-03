***TP Bus et Réseaux*** 

` `**Présentation :** 

La progression de ce travail pratique suivra la séquence suivante : tout d'abord, nous procéderons à l'interrogation des capteurs en utilisant le bus I²C. Ensuite, nous établirons la connexion  entre  le  STM32  et  le  Raspberry  Pi  pour  garantir  un  interfaçage  optimal.  Le troisième TP visera la création d'une interface web sur le Raspberry Pi. Enfin, le dernier TP se concentrera  sur  la  mise  en  place  d'une  interface  API  Rest  ainsi  que  sur  le  contrôle  des actionneurs via le bus CAN. 

**L'architecture du système :** 

![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.001.jpeg)

**TP1: Bus I2C** 

**Objectif** 

- **Interfacer un STM32 avec des un capteur I2C (BMP280)** 
1. **Mise en œuvre du BMP280** 

Le BMP280, conçu par Bosch, est un capteur de pression et de température qui utilise 

le protocole de communication I²C. Il est équipé de différents registres pouvant être modifiés ou lus pour assurer une configuration précise du capteur. Les registres utilisés comprennent : 

![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.002.png)

En  amorce,  la  première  étape  consiste  à  identifier  le  composant  (ID  =  0x58)  en utilisant  la  fonction  **BMP280\_check()** .  Cette  fonction  nous  autorise  à  lire  la  valeur  du registre ID à l'adresse 0xD0, et la valeur lue doit concorder avec 0x58, qui représente l'adresse du composant. 

Ensuite, nous configurons le capteur en mode normal avec une suréchantillonnage de la pression x16 et une suréchantillonnage de la température x2. Pour ce faire, nous utilisons la fonction ‘**BMP280\_init()**’, qui modifie le registre ctrl\_meas situé à l'adresse 0xF4. Les deux bits  LSB  nous  permettent  de  sélectionner  le  mode,  les  trois  bits  suivants  déterminent l'oversampling de la pression, et les trois bits MSB permettent de choisir l'oversampling de la température 

La calibration est réalisée à l'intérieur de la fonction  **BMP280\_get\_trimming(uint8\_t calibration\_data[Calibration\_size]**) . Cette fonction extrait les données stockées dans les registres  calib00  à  calib25  et  les  organise  dans  différents  tableaux.  Ces  données  seront ultérieurement  employées  pour  effectuer  la  compensation  nécessaire  ,  ‘ **bmp280\_compensate\_T\_int32()  et  bmp280\_compensate\_P\_int32**   ‘respectivement  pour Température et Pression . 

- La calibration ajuste les caractéristiques du capteur pour corriger les 

variations  de  fabrication,  tandis  que  la  compensation  ajuste  les  mesures  brutes  en tenant compte de la calibration pour garantir une précision optimale des données. Ces processus sont essentiels pour obtenir des résultats fiables et précis à partir du capteur BMP280. 

Nous sommes désormais en mesure de lire les valeurs de température et de pression qui  sont  stockées  dans  les  six  registres  suivants  du  BMP280  :  **« temp\_xlsb  ,  temp\_lsb ,temp\_msb , pour la température, et press\_xlsb , press\_lsb, press\_msb**» pour la pression, comme  indiqué  dans  la  datasheet  du  BMP280.  Il  est  important  de  noter  que  les  valeurs obtenues  sont  exprimées  en  hexadécimal,  correspondant  à  une  valeur  de  température d'environ  25°C.  Cependant,  la  dernière  étape  consiste  à  appliquer  et  adapter  l'algorithme spécifié dans la documentation afin d'obtenir une valeur en degrés Celsius. 

Dans ce but, nous avons mis en place deux fonctions distinctes : 

- Une fonction  **BMP280\_get\_temperature()**  qui renvoie la valeur de la température 

compensée en 100°C. 

- Une  fonction  **BMP280\_get\_pressure()**   qui  renvoie  la  valeur  de  la  pression 

compensée en Pascal (Pa). 

2. **Interfacer un STM32 avec des capteurs I2C** 

**Configuration du STM32** 

Dans STM32CubeIDE, nous avons configuré notre carte de développement STM32 selon ses spécifications. Pour ce travail pratique, nous avons réalisé les connexions suivantes : 

- Une  liaison  I²C  :  Idéalement,  utilisez  les  broches  compatibles  avec 

l'empreinte  Arduino,  telles  que  les  broches  PB6  et  PB7,  comme  indiqué  dans  la documentation de la carte Nucleo 64. 

- Une UART sur USB : l'UART2 avec les broches PA2 et PA3. 

![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.003.png)

![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.004.png)

![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.005.png)

` `**Redirection des sorties d'impression** 

Dans le but de faciliter le débogage de notre programme STM32, nous avons ajouté un code  au  fichier  **stm32f4xx\_hal\_msp.c**.  Cela  permet  à  la  fonction  printf  de  renvoyer correctement ses chaînes de caractères via la liaison UART sur USB. 

3. **Communication I²C** 
- A l’aide L'API HAL (Hardware Abstraction Layer) fournit par ST propose 

entre autres 2 primitives permettant d'interagir avec le bus I²C en mode Master: 

- HAL\_StatusTypeDef HAL\_I2C\_Master\_Transmit(I2C\_HandleTypeDef \*hi2c, uint16\_t DevAddress, uint8\_t \*pData, uint16\_t Size, uint32\_t Timeout) ![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.006.png)
- HAL\_StatusTypeDef HAL\_I2C\_Master\_Receive(I2C\_HandleTypeDef \*hi2c, uint16\_t DevAddress, uint8\_t \*pData, uint16\_t Size, uint32\_t Timeout) ![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.007.png)

**Communication avec le BMP280 :** 

Identification du BMP280 : consiste en la lecture du registre ID En I²C, la lecture se déroule de la manière suivante: 

- Envoyer l'adresse du registre ID 
- Recevoir 1 octet correspondant au contenu du registre 

**Configuration du BMP280 :** 

En I²C, l'écriture dans un registre se déroule de la manière suivante: 

- Envoyer l'adresse du registre à écrire, suivi de la valeur du registre 
- Si on reçoit immédiatement, la valeur reçue sera la nouvelle valeur du registre. 

**Résultats :**  

![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.008.png)

![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.009.png)

![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.010.png)

**TP2: Interfaçage STM32 – Raspberry** 

**Objectif** 

- **Permettre l'interrogation du STM32 via un Raspberry Pi Zero** 
1. **Mise en route du Raspberry PI Zéro** 

**Préparation du Raspberry** 

En premier lieu, on commence par télécharger l'image "Raspberry Pi OS (32-bit) Lite" et on l'installe sur la carte SD à l'aide de **Rpi\_Imager**. Ensuite, on procède à la configuration de l'image en  créant  les fichiers  **ssh**  et  **wpa\_supplicant.conf** dans la partition  boot  pour démarrer le système. 

Configuration réseau du routeur on a utilisé en TP : ![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.011.png)

SSID : ESE\_Bus\_Network Password : ilovelinux

**Configuration de l'image** 

Avec Windows on a utilisé Rpi\_imager 

Une fois la carte SD remontée, nous obtenons deux partitions : boot et rootfs. Pour rendre notre Raspberry accessible via SSH sur le réseau Wi-Fi, nous avons ajouté les fichiers suivants à la partition boot : 

- Un fichier nommé "ssh" dans la partition boot. Le contenu de ce fichier n'a pas 

d'importance,  mais  cela  permettra  de  démarrer  automatiquement  le  serveur  SSH  au démarrage. 

- Le fichier wpa\_supplicant.conf dans la partition boot. Qui contient ce code  

ctrl\_interface=DIR=/var/run/wpa\_supplicant GROUP=netdev update\_config=1 ![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.012.png)

country=FR 

network={ 

` `ssid="<Name of your wireless LAN>" 

` `psk="<Password for your wireless LAN>" 

}

- Pour activer le port série sur le connecteur GPIO, nous avons modifié le fichier 

config.txt dans la partition boot en ajoutant les lignes suivantes à la fin. 

- Pour activer le port série sur connecteur GPIO, sur la partition boot, modifiez ![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.013.png)

le fichier config.txt pour ajouter à la fin les lignes suivantes: 

enable\_uart=1 dtoverlay=disable-bt 

- la prochaine étape consiste à liberer le port UART en retirant l'option 

suivante dans le fichier cmdline.txt    **console=serial0,115200**  

- La dernière étape implique la modification des fichiers /etc/hostname et 

/etc/hosts dans la partition root pour changer le nom de la Raspberry. 

- Une fois la configuration terminée, nous pouvons commencer à élaborer 

notre  protocole  de  communication  avec  la  STM32.  Pour  cela,  la  première  étape consiste à connecter la STM32 à la Raspberry. Il est impératif de croiser les fils RX et TX, et surtout de ne pas oublier de connecter le GND de la Raspberry avec le GND de la STM32, car cela pourrait entraîner une perte considérable de temps. 

- Premier démarrage : Nous avons inséré la carte SD dans le Raspberry et l'avons alimenté. En utilisant "ssh", nous nous sommes connectés à notre Raspberry et nous avons obtenu notre adresse IP. 

![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.014.jpeg)

À ce point du TP, notre action se limite à l'affichage des valeurs de température et de pression en utilisant les commandes "**GET\_T**" et "**GET\_P**" sur Minicom. Nous répéterons cette démarche pour les autres commandes telles que "**GET\_K**", "**SET\_K**", et "**GET\_A**". 

2. **Commande depuis Python** 

On a débuté en mettant à jour avec la commande ‘**sudo apt update’**, puis en installant pip pour Python 3 via’ **sudo apt install python3-pip’**. Ensuite, on a procédé à l'installation du module pyserial avec la commande ‘**pip3 install pyserial’**. 

**o**  Nous avons rencontré un problème lors de l'écriture du code Python. 

Lors de l'utilisation de minicom, nous recevions un caractère via l'UART, effectuions un traitement, puis recevions un autre caractère. Étant donné que la vitesse d'écriture humaine  est  relativement  lente,  cela  fonctionnait  bien.  Cependant,  lorsque  nous envoyions des données en utilisant **ser.write**  avec une vitesse de transmission élevée, et étant donné que le traitement de chaque caractère prend du temps, il arrivait parfois que certains caractères soient perdus. 

Pour résoudre ce problème, nous avons opté pour l'utilisation de la fonction **HAL\_UARTEx\_ReceiveToIdle\_DMA(&huart3, RxBuffer, RX\_BUFFER\_SIZE**), qui  attend  de  recevoir  l'ensemble  des  caractères  avant  d'effectuer  le  traitement, assurant ainsi une meilleure synchronisation. 

**TP3: - Interface REST** 

**Objectif** 

- **Développement d'une interface REST sur le Raspberry** 
1. **Installation du serveur Python** 

Il faut d'abord créer son propre utilisateur avec les droits de sudo et  d'accès au port série (dialout) 

‘sudo adduser XXX’ 

‘sudo usermod -aG sudo XXX’ 

‘sudo usermod -aG dialout XXX’ 

Il faut ensuite installer le pack flask avec la commande ‘pip3 install flask’ qui va nous permettre de créer notre page REST 

2. **Première page REST avec métodes HTTP** 
- Nous avons créé notre serveur web et normalement on doit le lancer avec la commande ‘FLASK\_APP=nom\_du\_programme.py FLASK\_ENV=development flask run --host 0.0.0.0’ mais à cause de la contrainte de temps, nous n’avons pas pu exécuter cette partie.  

On choisit le format JSON pour les données car il est lisable par un homme ou une machine. Nous avons ajouté une page d'erreur 404 (html) avec la fonction ‘render\_template('page\_not\_found.html')’.   

Ensuite, nous avons ajouté les méthodes POST, GET, PUT, PATCH et DELETE pour définir les fonctions CRUD.   

**TP4: - Bus CAN** 

**Objectif** 

- **Développement d'une API Rest et mise en place d'un périphérique sur bus CAN** 
1. **Pilotage du moteur** 

Mise en œuvre d'un moteur pas-à-pas sur le bus CAN avec un baud rate de 500 kbits/s. Le paramètre du ratio seg2/(seg1+seg2) joue un rôle crucial dans la détermination du moment de la décision, et il est recommandé de le fixer à environ 87% pour éviter les erreurs de transmission. Nous avons opté pour un prescaler de 21, seg1 de 2 times et seg2 de 1 time. 

L'utilisation  du  bus  CAN  sur  la  STM32  nécessite  l'intermédiaire  d'un  transceiver CAN. Ce composant a été intégré sur une carte fille au format Arduino (shield) pour faciliter la connexion. Le shield est équipé d'un connecteur subd9, permettant de brancher un câble CAN compatible avec le moteur. 

Les pins RX et TX du transceiver sont initialement connectés à PB8 et PB9 de la carte Nucleo. Il est donc nécessaire de reconfigurer ces connexions pour assurer la communication entre la STM32 et la Raspberry Pi. 

![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.015.png)

![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.016.png)

Pour mettre en marche le module CAN, nous allons employer la fonction 

‘ **HAL\_CAN\_Start(&hcan1)** . 

En ce qui concerne l'envoi d'un message, nous utiliserons la fonction 

` `**HAL\_CAN\_AddTxMessage(&hcan1, &pHeader, aData, &pTxMailbox)** . 

- Le moteur opère selon deux modes distincts : automatique et manuel 
- **Mode manuel :** 

Ce mode vous permet d'envoyer une trame CAN complexe pour contrôler : 

- Le nombre d'étapes 
- La direction de rotation 
- La vitesse du moteur 

![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.017.png)

- **Mode automatique :** 

Mode de calcul automatique d'angle :  vous devez simplement  envoyer une trame CAN configurée avec 

l'angle du moteur, puis le moteur tournera jusqu'à ce que le nouvel angle soit atteint. 

Composition de la trame : 

- Donnée D0 : Angle souhaité, entre 0° et 180° ( >180° sera considéré comme 

180° ) 

- Donnée D1 : Représente le signe de l'angle 
- Par exemple, D0 = 0x54 – D1 = 0x01 – Résulte en un angle de -90° 

Ainsi, pour faire tourner le moteur à un angle spécifique, nous utiliserons le mode automatique en ajustant le  **pHeader**  pour sélectionner l'ID d'arbitrage, et nous choisirons  l'angle  et  le  signe  en  modifiant  le  tableau  aData.  Par  exemple,  pour effectuer une rotation de 90° dans le sens positif : 

- aData[0] doit être réglé sur 90. 
- aData[1] doit être réglé sur 0. 
2. **Interfaçage avec le capteur** 

L'objectif de cette section est de faire tourner le moteur en fonction de l'évolution de la température, où le coefficient **k** détermine l'angle de rotation pour chaque degré Celsius. Pour ce  faire,  nous  avons  d'abord  configuré  le  bus  CAN.  Ensuite,  nous  avons  récupéré  la température à l'aide du capteur BMP280 en utilisant un timer qui génère une interruption toutes les dixièmes de seconde. Ainsi, la température est collectée toutes les dixièmes de seconde. 

**Résultats :** 

![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.018.png)

**TP5:  - Integration I²C - Serial - REST - CAN** 

**Objectif** 

- **Faire marcher ensemble les TP 1, 2, 3 et 4** 

À ce point, la Raspberry interroge la STM32, qui lui fournit les données du 

capteur. En conséquence, la STM32 régule le moteur pas-à-pas de manière  proportionnelle.

![](Aspose.Words.3a5b7c8a-d059-45df-bf65-ea8c09280619.019.png)

Tout d'abord, nous avons créé l'URL de la température à l'aide de la fonction **@app.route('/temp/',  methods=['GET',  'POST'])**.  À  l'intérieur  de  cette  fonction, nous  avons  défini  les  deux  méthodes  possibles,  **GET**  et  **POST**,  puis  nous  avons spécifié ces méthodes dans **def api\_temp().** 

Lorsqu'on fait appel à la méthode **GET**, l'objet renvoyé est un JSON contenant la température, l'unité, ainsi que le timestamp. Nous stockons ensuite cet objet JSON dans une liste de JSON pour obtenir, à terme, une liste de toutes les températures relevées à chaque appel de la méthode  GET  température. Nous effectuons la même procédure pour la pression. 

Quant au coefficient K, nous avons suivi une approche similaire en utilisant **@app.route('/scale/<int:K>', methods=['POST']).** Cependant, dans ce cas, la seule méthode possible est un **POST** qui permet de modifier la valeur du coefficient K. 

Pour vérifier le bon fonctionnement de l'ensemble, il suffit d'utiliser l'extension REST de **Firefox.** Dans un premier temps, nous effectuons un **GET** de la température et de la pression. Nous pouvons également changer la valeur du coefficient  **K** en utilisant la méthode **POST**, ce qui nous permet de  selon la valeur de **K**, le moteur tourne plus ou moins rapidement. 

**En conclusion,**  

Ce TP a été très instructif pour les étudiants. Nous avons pu mettre en œuvre différents protocoles de communication, à savoir : I²C et CAN. De plus, nous avons exploré les aspects des réseaux et des API REST, bien que certaines parties de REST aient été préparées et que, faute de temps, nous n'ayons pas pu les expérimenter. Nous espérons que les étapes et la mise en œuvre réelle fonctionnent comme prévu. 
