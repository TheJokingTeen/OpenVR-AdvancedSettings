#pragma once
#include "input_parser.h"
#include <easylogging++.h>

enum class KeyStatus
{
    Up,
    Down,
};

void initOsSystems();
void shutdownOsSystems();

void sendKeyPress( const Token token, const KeyStatus status );

inline void sendTokensAsInput( const std::vector<Token> tokens )
{
    initOsSystems();

    std::vector<Token> heldInputs = {};
    bool noKeyUp = false;
    for ( const auto& token : tokens )
    {
        if ( isModifier( token ) )
        {
            sendKeyPress( token, KeyStatus::Down );
            if ( noKeyUp )
            {
                heldInputs.push_back( token );
            }
            else
            {
                sendKeyPress( token, KeyStatus::Up );
            }
            continue;
        }
        if ( isLiteral( token ) )
        {
            sendKeyPress( token, KeyStatus::Down );
            if ( noKeyUp )
            {
                heldInputs.push_back( token );
            }
            else
            {
                sendKeyPress( token, KeyStatus::Up );
            }
            continue;
        }
        // NEEDS to be checked after modifer check and literal? but should never
        // be called on literal?
        if ( token == Token::TOKEN_NO_KEYUP_NEXT )
        {
            noKeyUp = true;
            continue;
        }
        else
        {
            noKeyUp = false;
        }

        if ( token == Token::TOKEN_NEW_SEQUENCE )
        {
            for ( const auto& h : heldInputs )
            {
                sendKeyPress( h, KeyStatus::Up );
            }
            heldInputs.clear();
            continue;
        }
    }

    if ( !heldInputs.empty() )
    {
        for ( const auto& h : heldInputs )
        {
            sendKeyPress( h, KeyStatus::Up );
        }
    }

    shutdownOsSystems();
}

inline void sendTokenPress( const Token token, KeyStatus event = KeyStatus::Up )
{
    initOsSystems();

    sendKeyPress( token, event );

    shutdownOsSystems();
}

inline void sendFirstCharAsInput( const std::string inputstring,
                                  KeyStatus event )
{
    const auto tokens = ParseKeyboardInputsToTokens( inputstring );
    const auto inputs = removeIncorrectTokens( tokens );
    if ( !tokens.empty() )
    {
        sendTokenPress( inputs.front(), event );
    }
}

inline void sendStringAsInput( const std::string input )
{
    const auto tokens = ParseKeyboardInputsToTokens( input );
    const auto inputs = removeIncorrectTokens( tokens );
    sendTokensAsInput( inputs );
}
