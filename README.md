
seb-ledger supports a subset of features in http://ledger-cli.org/

With seb-ledger, however, a transaction can query the balance of an account inside a posting.


Simple posting
--------------

```
2017-07-15 Potatoes
    store  99.00 CAD
    seb  -99.00 CAD
```

    
Posting with mathematical equations
-----------------------------------

```
2017-07-15 Tomatoes
    store2  12.00 CAD
    seb  -12.00 CAD
    special  (12.00/2 CAD)
    auto
```

Posting with a account balance query
------------------------------------

```
2017-07-15 Interests
    mortgage  (mortgage*0.03)
    auto
```


Commands
--------

```
seb-ledger --file ledger-files/master.ledger balance
seb-ledger --file ledger-files/master.ledger balance seb-receivables-from-joe-CAD
seb-ledger --file ledger-files/master.ledger register seb-receivables-from-joe-CAD
```
