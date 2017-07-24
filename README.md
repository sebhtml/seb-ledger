
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

Examples
--------

```
$ seb-ledger --file file1.ledger balance
bank       -1000.00 CAD
dummy         -7.50 CAD
loan        1002.50 CAD
max-part       5.00 CAD
seb          -10.00 CAD
store         10.00 CAD
```

```
$ seb-ledger --file file1.ledger register loan
2017-07-01  loan        1000.00 CAD    1000.00 CAD   Loan
2017-07-01  loan           1.25 CAD    1001.25 CAD   Interests
2017-07-15  loan           1.25 CAD    1002.50 CAD   Interests
```
