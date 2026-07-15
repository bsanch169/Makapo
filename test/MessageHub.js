const serverIP = "192.168.4.1";

const readline = require("readline");

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

function printMenu() {
    console.log("\n\n---MENU---\n");
    console.log("1. Display boat data");
    console.log("2. Display paddler data");
    console.log("3. Send a preset message");
    console.log("4. Set a custom message");
    console.log("5. Exit");
}

function handleChoice(choice) {
    switch (choice.trim()) {
        case "1":
            console.log("Display boat data selected.");

            rl.question("Enter boat ID: ", function(boatID) {
                fetch(`http://${serverIP}/${boatID.trim()}`)
                    .then(function(response) {
                        return response.text();
                    })
                    .then(function(data) {
                        printResponse(data);
                        promptUser();
                    })
                    .catch(function(error) {
                        console.log("\nError getting boat data:");
                        console.log(error.message);
                        promptUser();
                    });
            });

            return;

        case "2":
            console.log("Display paddler data selected.");

            rl.question("Enter boat ID: ", function(boatID) {
                rl.question("Enter paddler ID: ", function(paddlerID) {
                    fetch(`http://${serverIP}/${boatID.trim()}/${paddlerID.trim()}`)
                        .then(function(response) {
                            return response.text();
                        })
                        .then(function(data) {
                            printResponse(data);
                            promptUser();
                        })
                        .catch(function(error) {
                            console.log("\nError getting paddler data:");
                            console.log(error.message);
                            promptUser();
                        });
                });
            });

            return;

        case "3":
            console.log("Send preset message selected.");

            rl.question("Enter boat ID: ", function(boatID) {
                rl.question("Enter message ID: ", function(messageID) {
                    fetch(`http://${serverIP}/message/${boatID.trim()}/${messageID.trim()}`, {
                        method: "POST"
                    })
                        .then(function(response) {
                            return response.text();
                        })
                        .then(function(data) {
                            printResponse(data);
                            promptUser();
                        })
                        .catch(function(error) {
                            console.log("\nError sending preset message:");
                            console.log(error.message);
                            promptUser();
                        });
                });
            });

            return;

        case "4":
            console.log("Set custom message selected.");

            rl.question("Enter boat ID: ", function(boatID) {
                rl.question("Enter custom message, max 40 characters: ", function(customMessage) {
                    customMessage = customMessage.trim();

                    if (customMessage.length > 40) {
                        console.log("\nError: Custom message must be 40 characters or less.");
                        promptUser();
                        return;
                    }

                    fetch(`http://${serverIP}/message/0/${boatID.trim()}`, {
                        method: "POST",
                        headers: {
                            "Content-Type": "application/json"
                        },
                        body: JSON.stringify({
                            message: customMessage
                        })
                    })
                        .then(function(response) {
                            return response.text();
                        })
                        .then(function(data) {
                            printResponse(data);
                            promptUser();
                        })
                        .catch(function(error) {
                            console.log("\nError sending custom message:");
                            console.log(error.message);
                            promptUser();
                        });
                });
            });

            return;

        case "5":
            console.log("Exiting...");
            rl.close();
            return;

        default:
            console.log("Invalid option. Please enter 1, 2, 3, 4, or 5.");
            break;
    }

    promptUser();
}

function promptUser() {
    printMenu();

    rl.question("\nSelect an option: ", function(choice) {
        handleChoice(choice);
    });
}

function printResponse(data) {
    console.log("\nServer response:");

    try {
        const parsed = JSON.parse(data);
        console.log(JSON.stringify(parsed, null, 4));
    } catch (error) {
        console.log(data);
    }
}

promptUser();