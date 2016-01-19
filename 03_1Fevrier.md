### Analyse des outils nécessaires pour l'accomplissement du projet ###

**Matériel**

Le CerfPDA qu'utilise le livreur affichera une interface réalisée grâce à uC/GUI et permettra notamment au livreur d'entrer un numéro de colis à l'aide du clavier. Cette fonctionnalité sera utile lors d'une cueillette de colis, alors qu'aucun code-barres n'est présent sur le colis. En effet, le PDA sera équippé d'un lecteur de code-barres afin d'identifier rapidement un colis. Le fait de saisir un code-barres permet au livreur de changer l'état (non cueilli, cueilli, en livraison, livré) de ce dernier ou de tout simplement lire ses informations.

Le PDA sera aussi équippé d'un module GPS permettant à la centrale de connaître la position du camion en tout temps. Les coordonnées GPS du camion serviront aussi à générer des cartes montrant au livreur l'itinéraire qu'il doit suivre. La communication entre la centrale et le PDA sera établie grâce à un module cellulaire. Un tel module a une beaucoup plus grand portée qu'un module RF. Comme nous avons besoin d'établir entre des endroits possiblement situés aux extrémités d'une grande ville, une telle portée nous est nécessaire.

**Logiciel**

Il a été décidé que l'environnement de développé utilisé sera Microsoft Visual Studio 2005. Cela permettra à l'équipe d'éviter des conflits de librairies de développement. L'interface utilisée par la centrale (donc présente sur le serveur) sera réalisée en langage ASP.NET et C#. Nous emploierons une base de données MySQL.

Les documents seront produits grâce à la suite Microsoft Office afin de conserver une uniformité dans les documents. Le code source sera conservé sur un serveur Subversion (SVN) accessible à l'adresse code.google.com/p/pdale. Ce même site contiendra notre logbook, présenté sous forme de wiki qui contiendra les résumés de nos rencontres d'équipe.