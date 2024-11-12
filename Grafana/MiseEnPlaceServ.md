## Étapes d'installation

### 1. Mise à jour du système

Commencez par mettre à jour votre système pour vous assurer que tous vos paquets sont à jour. Ouvrez le terminal et exécutez :

bash

`sudo apt-get update && sudo apt-get upgrade -y`

### 2. Ajout du dépôt Grafana

Ajoutez le dépôt officiel Grafana à votre liste de sources :

bash

`echo "deb https://packages.grafana.com/oss/deb stable main" | sudo tee -a /etc/apt/sources.list.d/grafana.list`

### 3. Ajout de la clé GPG

Ajoutez la clé GPG pour s'assurer de l'authenticité des paquets :

bash

`curl https://packages.grafana.com/gpg.key | sudo apt-key add -`

### 4. Installation de Grafana

Mettez à jour votre liste de paquets puis installez Grafana :

bash

`sudo apt-get update sudo apt-get install grafana -y`

### 5. Démarrage de Grafana

Activez et démarrez le service Grafana :

bash

`sudo systemctl enable grafana-server sudo systemctl start grafana-server`

### 6. Vérification du statut de Grafana

Pour vérifier que Grafana fonctionne correctement, utilisez :

bash

`sudo systemctl status grafana-server`

Vous devriez voir un statut indiquant que le service est actif et en cours d'exécution.
