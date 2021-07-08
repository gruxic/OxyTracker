const express = require('express');
const mysql = require("mysql2/promise");
const app = express();
let port =process.env.PORT || 3000;

const insertIntoDB = async (SpO2_value,bpm_value) =>{
    const connection = await mysql.createConnection({
        host: "sql6.freesqldatabase.com",
        user: "sql6422421",
        password: "9su1EKKgZE",
        database: "sql6422421",
        port: 3306,
    })

    try{
        await connection.query(
            "INSERT INTO healthData (SpO2,bpm) VALUES(" + SpO2_value + "," + bpm_value + ");"
        )
        console.log("Inserted");
    }catch(e){
        console.log(e);
    }
};


app.get('/:SpO2/:bpm',(req,res) => {
    var oxydata = parseInt(req.params.SpO2);
    var bpmdata = parseInt(req.params.bpm);
    if((!isNaN(oxydata))&&(!isNaN(bpmdata)))
    {
        console.log('SpO2: '+oxydata+"bpm:"+bpmdata);
        res.send('SpO2: '+oxydata+"& bpm:"+bpmdata);
        insertIntoDB(req.params.SpO2,req.params.bpm);
    }
    else{
        res.send("ER:WRNG DATA"); //trying to use less than 16 characters so that it is visible on the 16*2 LCD screen
    }

});
app.get('/',(req,res) =>{
    res.send("USE GET METHOD TO INSERT DATA");
});

app.listen(port,()=>{
    console.log('Server runnning!');
});