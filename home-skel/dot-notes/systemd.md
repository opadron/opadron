systemd
-------

 - (**arch linux**)

### usage

##### basic usage
```Shell
systemctl $OP $UNIT
```
**OP**:
 - `start`
 - `stop`
 - `restart`
 - `reload`
 - `status`  
 - `enable`  
   run at boot
 - `disable`
 - `is-enabled`
 - `help`

##### scan for unit changes
```Shell
systemctl daemon-reload
```

##### power management (requires `polkit`)
```Shell
systemctl $OP
```
**OP**:
 - `poweroff`
 - `reboot`
 - `suspend`
 - `hibernate`
 - `hybrid-sleep`

##### listing units
```Shell
systemctl [list-units]
systemctl --failed
```

### units

##### unit file path
`/usr/lib/systemd/system`  
`/etc/systemd/system`

##### installed units
```Shell
systemctl list-unit-files
```

##### unit types
`.service` `.mount` `.device` `.socket`

##### referring to units
`name.service` or `name`  
`home.mount` or `/home`  
`dev-sda2.device` or `/dev/sda2`
`name.target`

**unit template instance**: `name@string.suffix`  
(`string` passed to `name@.suffix` as `%i`)

### writing .service files
```Shell
[Unit]
Description=$UNIT_DESCRIPTION
(Requires|Wants)=$DEPENDENCY
After=$DEPENDENCY
DefaultDependencies=true

[Service]
Type=(simple|forking|oneshot|notify|dbus)
PIDFile=/var/run/tpfand.pid
ExecStart=/usr/sbin/tpfand

[Install]
WantedBy=multi-user.target
```

#### Type
 - `simple` - started up immediately. process must not fork
 - `forking` - traditional forking process  
   `PIDFile=$DEAMON_PID_FILE`
 - `oneshot` - run and exit  
   `RemainAfterExit=yes`  service is "active" after finishing
 - `notify` - daemon signals systemd when ready
   (`libsystemd-daemon`)
 - `dbus` - ready when a bus appears on DBus's system bus

#### extending unit files
put `.conf` files in
```Shell
/etc/systemd/system/$UNIT.d
```

`systemd` will patch units with the provided additions

#### Targets

`.target` units - serve as groupings and clear synchronization points.  Typically used to replace sysv

### Links
 - [systemd.unit(5)](http://www.freedesktop.org/software/systemd/man/systemd.unit.html)
 - [unit examples](https://wiki.archlinux.org/index.php/Systemd/Services)