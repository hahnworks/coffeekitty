# ☕🫰😺 coffeekitty

A terminal-based management tool for collective coffee spending

## Idea

I developed this to ease the tracking of coffee consumption in our workgroup.
You can create a tally sheet with the persons currently drinking coffee, where every person has to add a tally when making coffee.

![coffeekitty latex](img/coffeekitty_print.png)

At the end of an accounting period, you can enter the coffee consumption, where `coffeekitty` will then calculate the balance (and adjust the height of the tally sheet).

![coffeekitty print](img/coffeekitty_latex.png)

Coffeekitty also tracks the number of packs of coffee in storage.


## Dependencies

Just `libxml2` and `pkg-config`. You will need `pdflatex` for the tally sheet.


## Installation

```
make
make install
```


## Usage

You can access the list of commands using `coffeekitty help`.

### Basics

Globally, there is a coffeekitty containing money and packs of coffee.
You can add and remove persons from the list of coffee enthusiasts using

```bash
coffeekitty add Alice
coffeekitty add Bob
coffeekitty add Carol
# ...
coffeekitty remove Alice
```

You can print the state of the coffeekitty using `coffeekitty print`.
This is the default behaviour.

A cup of coffee has a fixed price, which defaults to 0.25 EUR.
You can set the price using `coffeekitty set price 1.00`.
The currency display can be adjusted for e.g. USD using `coffeekitty set currency USD` and `coffeekitty set prefix_currency_value true`.

Create a tally using

```bash
coffeekitty latex > sheet.tex && pdflatex sheet.tex
```

This can be hung above your coffee machine. If a person drinks coffee, she indicates it by adding a tally.


### Payment and Balance

Every person has a balance. If a person drinks coffee, it can be indicated by

```bash
coffeekitty drink [name] [amount]
```

This automatically updates the person’s balance.

If a person pays her debts, you can use

```bash
coffeekitty pay [name] [amount]
```

The balance of the coffeekitty and the balance of the person are updated accordingly.

### Thirst

When adding the drank coffees, they increase the counter of `current coffees` for a person.
By using `coffeekitty thirst` you can invoke a calculation of a thirst parameter, which is the amount of coffee consumed by this one person in the last period.
The thirst parameter is used for the line-height calculation of the tally sheet.
After `coffeekitty thirst` is called, the `current coffees` counter is reset.

### Coffee Pack Management

Use

```bash
coffeekitty consume
```

if a pack has been used up.

You can use

```bash
coffeekitty buy [packs] [cost]
```

to use money in the kitty to update the packs in the kitty.


## Troubleshooting

### Database

If something goes wrong, you can modify the database located in `$HOME/.coffeekitty/data.xml`.


### (Current) Limitations

No transaction log has been implemented yet, therefore there is no undo function.