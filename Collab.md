# 📚 Guide de collaboration GitHub - Projet DEV

Bienvenue sur le dépôt GitHub de notre équipe !  
Voici la méthode officielle pour collaborer proprement sur ce projet.

---

## 🚀 1️⃣ Cloner le dépôt

Commence par cloner le projet sur ton PC :

```bash
git clone https://github.com/TON-ORGANISATION/TON-REPO.git
```

---

## 🌿 2️⃣ Créer ta branche personnelle

Ne travaille JAMAIS directement sur `main`.  
Crée ta branche comme ceci :

```bash
git checkout -b tonprenom-dev/ta-fonction
```

Exemple :

```bash
git checkout -b alex-dev/frontend-login
```

---

## 🔀 3️⃣ Se déplacer vers une branche

Si ta branche est déjà créée et que tu veux y revenir :

```bash
git checkout tonprenom-dev/ta-fonction
```

Exemple :

```bash
git checkout alex-dev/frontend-login
```

---

## 💾 4️⃣ Ajouter et valider ton code

Quand tu as modifié des fichiers, fais ces commandes :

1. **Ajouter tes fichiers :**

```bash
git add .
```

2. **Créer un commit clair :**

```bash
git commit -m "[Feature] : Ajout du formulaire de login"
```

> [Fix] : Correction du bug d'affichage du bouton <br>
> [Feature] : Ajout du système d'authentification <br>
> [Refactor] : Réorganisation du code du composant Navbar


---

## 📡 5️⃣ Envoyer ta branche vers GitHub

Pousse ta branche sur le dépôt :

```bash
git push origin tonprenom-dev/ta-fonction
```

Exemple :

```bash
git push origin alex-dev/frontend-login
```

---

## 🔄 6️⃣ Mettre à jour ta branche avec `main`

Avant de faire une Pull Request, assure-toi d'avoir le dernier code stable :

```bash
git checkout main
git pull origin main
git checkout tonprenom-dev/ta-fonction
git merge main
```

Résous les éventuels conflits, puis :

```bash
git add .
git commit -m "[Fix] : Résolution de conflits avec main"
git push origin tonprenom-dev/ta-fonction
```

---

## 📥 7️⃣ Faire une Pull Request (PR)

Quand ta fonctionnalité est prête :

1. Va sur GitHub.
2. Clique sur **Compare & Pull Request**.
3. Décris **clairement** ce que tu as fait.
4. Attends que **2 membres minimum fassent une Code Review**.

---

## 👀 8️⃣ Code Review

Les autres doivent lire ton code, vérifier :  
🔢 Qu'il fonctionne  
🔢 Qu'il est clair et propre  
🔢 Qu'il suit la convention  

Ils valident avec :  
- **"Approved"** si c'est bon.  
- Commentaires si besoin de modifications.

---

## 🔀 9️⃣ Merge vers `main`

Une fois validé par au moins 2 membres :  
🔢 La PR peut être mergée dans `main`.

---

## 🗓 1️⃣️0️⃣ Bonnes pratiques

- **1 tâche = 1 branche = 1 PR**.
- Toujours faire un `git pull` avant de commencer.
- Commit clair et précis.
- Pas de code en `main` sans Pull Request.

---

💡 En respectant cette méthode, votre code sera propre, stable et facile à relire par tout le monde !

