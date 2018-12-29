
function isEmpty(str){
    return (!str || 0 === str.length);
}

function checkIp(ip) {
    if(ip === "ANY")
        return true;
    var reg = /^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/
    return reg.test(ip);
}

function checkMask(mask){
    if(mask === "N/A")
        return true;
    var i = parseInt(mask);
    if(mask > 0 && mask <= 32){
        return true;
    }else{
        return false;
    }
}

function checkPort(port){
    if(port === "N/A"){
        return true;
    }
    var reg1 = /^([0-9]|[1-9]\d{1,3}|[1-5]\d{4}|6[0-5]{2}[0-3][0-5])$/
    var reg2 = /^([0-9]|[1-9]\d{1,3}|[1-5]\d{4}|6[0-5]{2}[0-3][0-5]):([0-9]|[1-9]\d{1,3}|[1-5]\d{4}|6[0-5]{2}[0-3][0-5])$/
    var reg3 = /ANY/
    return reg1.test(port) || reg2.test(port) || reg3.test(port);
}


function check(from, type, str){
        var ret, show_str;
        switch(type){
            case "ip":
                ret = checkIp(str);
                if(ret){
                    if(from === "source"){
                        qrule.source_ip = str;
                    }else if(from === "dest"){
                        qrule.dest_ip = str;
                    }else if(from === "before"){
                        qnat.before_ip = str;
                    }else{
                        qnat.after_ip = str;
                    }
                    return;
                }
                break;
            case "mask":
                ret = checkMask(str);
                if(ret){
                    if(from === "source"){
                        qrule.source_mask = str;
                    }
                    else if(from === "dest"){
                        qrule.dest_mask = str;
                    }
                    else if(from === "before"){
                        qnat.before_mask = str;
                    }else{
                        qnat.after_mask = str;
                    }
                    return;
                }
                break;
            case "port":
                ret = checkPort(str);
                if(ret){
                    if(from === "source"){
                        qrule.source_port = str;
                    }
                    else if(from === "dest"){
                        qrule.dest_port = str;
                    }else if(from === "before"){
                        qnat.before_port = str;
                    }
                    else{
                        qnat.after_port = str;
                    }
                    return;
                }
                break;
        }
        show_str = from + " " + type;
        if(isEmpty(str)){
            return [1, show_str];
        }
        else{
            return [2, show_str];
        }
    }
