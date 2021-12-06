# System Programming - Project 2 - 2021

## Name: Ioannis
## Surname: Georgopoulos
## sdi:1115201800026

###           Το readme.txt περιέχει σχόλια όσων αναφορά την αποστολή δεδομένων μέσω pipes καθώς και σχολιασμό πανώ στην λειτουργία των signals τόσο στο travelmonitor όσο και στα monitors

Pipes:
======

Αποστολή integers:
==================
Αν το bufferSize είναι μεγαλύτερο ή ίσο του 4 απλά στένλω το int. Αν είναι μικρότερο του 4 και δεδομένου ότι 
το μικρότερο μέγεθος buffer που μπορεί να βάλεi ο χρήστης είναι 2 όπως αναφέρθηκε στο piazza, χωρίζω το int σε δυο
κομμάτια και τα αποθηκεύω σε short ints(2bytes) και μετά τα στέλνω και από την πλευρά τα επανασυνθέτω με bitwise or.
Αποστολή strings:
=================
Για την αποστολή strings μέσω pipes αν το string είναι μικρότερο του bufferSize χωρίζω το string σε μικρότερα
κομμάτια μεγέθους buffersize. Αν το string είναι μικρότερο του bufferSize απλά στέλνει το string όπως έχει.
Ο παραλήπτης λαμβάνει πρώτα μέσω pipe το αριθμό των κομματιών που χωρίστηκε το string και έπειτα δίαβαζει ένα ένα
τα κομμάτια. Στέλνω τον αριθμό των κομματιών για να ξέρει ο παραλήπτης πόσα reads να κάνει.
Αποστολή bloomfilter:
=====================
Για την αποστολή βλέπω πόσα byte είναι το bloomfilter και στέλνω όλα τα byte one by one.

Signals:
========
TravelMonitor:
==============
SIGCHLD:
Με αυτό το singal ελέγχω αν ένα παιδί έχει πεθάνει. Αν σταλθεί αυτο το signal τότε αλλάζω το global flag flag_chldsig
και στην επόμενη εντολή που θα δεχτεί θα κάνει detect το deleted child και θα το επαναφέρει.

Μια άλλη λειτουργία του SIGCHLD είναι όταν δίνει το σήμα SIGKILL σε ένα monitor περιμένει να σταλθεί πίσω σήμα SIGCHLD
ότι όντως τερματισε το monitor που εγινε SIGKILL.

SIGINT,SIGQUIT:
Όταν σταλούν ένα αυτά τα μυνήματα στο travelmonitor τότε αλλάζω το αντίστιχο global flag flag_intsig ή flag_intsig τότε το
travelmonitor βλέπει ότι έχουν αλλαξει τα flags και αρχίζει να κλείνει ομαλά. Απλά πρέπει να δωθεί κάποια εντολή η κάποιο enter.
Μαζί με αυτό τα κλείσει και

Monitor:
========
SIGINT,SIGQUIT:
Όταν σταλούν ένα αυτά τα μυνήματα στο monitor από το travelmonitor τότε αλλάζω το αντίστιχο global flag flag_intsig ή flag_intsig τότε το
κάθε monitor βλέπει ότι έχουν αλλάξει τα flags και θα δημιουργηθεί το log_file.xxx . Απλά πρέπει να δωθεί κάποια εντολή από το TravelMonitor.

Μεταγλωτηση και εκτέλεση:
=========================
Η μεταγλώτηση γίνεται με το makeFile
Η εκτέλεση γίνεται με το ./bin/travelMonitor -m numOfMonitors -b bufferSize -s bloomfiltersize -i inputDirectory

Επιπλέον σχόλια:
====
Μερικές φορές μπορεί να χρειάζεται να γινει chmod 777 FiFos
