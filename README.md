# ☕🫰😺 coffeekitty

A terminal based managment tool for collective coffee spending


## Dependenices

Just `gettext`, `libxml2` and `pkg-config`.
Most likely all three are already installed.


## Installation

```
make
make install
```


## Usage

### Current state

You can always use

```bash
coffeekitty print
```

to get the current state of the coffeekitty.
If coffeekitty is ran without arguments, this is the default behaviour.

### Drinking

Globally, there is a coffeekitty containing money and packs of coffee.
A cup of coffee has a fixed price, which defaults to 0.25 EUR.

Add persons using `coffeekitty add [name]`.
Every person has a balance. If a person drinks coffees, it can be indicated by

```bash
coffeekitty drink [name] [amount]
```

This automatically updates the person’s balance.


### Payment

If a person pays her debts, you can use

```bash
coffeekitty pay [name] [amount]
```
The balance of the coffeekitty and the balance of the person are updated accordingly.


### Replenishing coffee

Use 

```bash
coffeekitty buy [packs] [cost]
```

to use money to update the packs in the kitty.