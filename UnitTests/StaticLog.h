#ifndef STATICLOG2_H
#define STATICLOG2_H

#include "Catch2/Catch.hpp"
#include "Libraries/Fmt/fmt/os.h"
#include "Libraries/Fmt/fmt/printf.h"
#include "GClasses/Common/StaticLog.h"


//#include "gtest/gtest.h"
namespace StaticLogger
{
    using namespace StaticLogTesting;

    namespace LoggerInterfaceTesting
    {
        TEST_CASE("CRTP overloading", "[LoggerInterface]")
        {
            //StaticLogger::LoggerInterface<StaticLogger::TestingLogger1>  logWriter1;
            LoggerInterface<TestingLogger1> logWriter1;
            REQUIRE(logWriter1.Identifier == "TestingDataPackage");
            REQUIRE(logWriter1.LastFunctionCalled.empty() == true);

            LoggerInterface<TestingLogger2> logWriter2;
            REQUIRE(logWriter2.Identifier == "TestingDataPackage2");
            REQUIRE(logWriter2.LastFunctionCalled.empty() == true);
            REQUIRE(logWriter2.MyInt1 == 1);
            REQUIRE(logWriter2.MyInt2 == 2);
            REQUIRE(logWriter2.MyInt3 == 3);

            //LoggerInterface<TestingLogger2, TestingDataPackage1 > invalidWriter; //Should not compile!!!

        }

        TEST_CASE("CRTP overloaded copy constructors and assignments", "[LoggerInterface]")
        {
            GIVEN("Two logWriters with copy constructors and assignments")
            {
                TestingDataPackageCopy dataPackageCopy(32, "Copy");

                REQUIRE(dataPackageCopy.Str == "Copy");
                REQUIRE(dataPackageCopy.LineNum == 32);

                LoggerInterface<TestingLoggerCopy> logWriterCopy1(11, "Copy1");
                LoggerInterface<TestingLoggerCopy> logWriterCopy2(22, "Copy2");
                logWriterCopy1.DoLog(false);
                logWriterCopy1.DoLoggingLevels<1, 3>(false);

                WHEN("Copy constructing from package")
                {
                    LoggerInterface<TestingLoggerCopy> fromPackage((const TestingDataPackageCopy)dataPackageCopy);
                    THEN("The contents will be copied")
                    {
                        REQUIRE(fromPackage.Str == "Copy");
                        REQUIRE(fromPackage.LineNum == 32);
                        REQUIRE(dataPackageCopy.Str == "Copy");
                        REQUIRE(dataPackageCopy.LineNum == 32);
                    }
                }

                WHEN("Copy constructing from logWriter")
                {
                    LoggerInterface<TestingLoggerCopy> copyConstructed(logWriterCopy1);
                    THEN("The contents will be copied")
                    {
                        REQUIRE(copyConstructed.Str == "Copy1");
                        REQUIRE(copyConstructed.LineNum == 11);
                        REQUIRE(logWriterCopy1.Str == "Copy1");
                        REQUIRE(logWriterCopy1.LineNum == 11);

                        REQUIRE(logWriterCopy1.IsLogEnabled() == false);
                        REQUIRE(logWriterCopy1.IsLogEnabled(1) == false);
                        REQUIRE(logWriterCopy1.IsLogEnabled(3) == false);

                        REQUIRE(copyConstructed.IsLogEnabled() == false);
                        REQUIRE(copyConstructed.IsLogEnabled(1) == false);
                        REQUIRE(copyConstructed.IsLogEnabled(3) == false);
                    }
                }

                WHEN("Copy assigning from logWriter")
                {
                    logWriterCopy2 = logWriterCopy1;
                    THEN("The contents will be copied")
                    {
                        REQUIRE(logWriterCopy2.Str == "Copy1");
                        REQUIRE(logWriterCopy2.LineNum == 11);
                        REQUIRE(logWriterCopy1.Str == "Copy1");
                        REQUIRE(logWriterCopy1.LineNum == 11);

                        REQUIRE(logWriterCopy2.IsLogEnabled() == false);
                        REQUIRE(logWriterCopy2.IsLogEnabled(1) == false);
                        REQUIRE(logWriterCopy2.IsLogEnabled(3) == false);

                        REQUIRE(logWriterCopy1.IsLogEnabled() == false);
                        REQUIRE(logWriterCopy1.IsLogEnabled(1) == false);
                        REQUIRE(logWriterCopy1.IsLogEnabled(3) == false);
                    }
                }
            }
        }

        TEST_CASE("CRTP overloaded move constructors and assignments", "[LoggerInterface]")
        {
            GIVEN("Two logWriters with move constructors and assignments")
            {
                TestingDataPackageMove dataPackageMove(32, "Move");

                REQUIRE(dataPackageMove.Str == "Move");
                REQUIRE(dataPackageMove.LineNum == 32);

                LoggerInterface<TestingLoggerMove> logWriterMove1(11, "Move1");
                LoggerInterface<TestingLoggerMove> logWriterMove2(22, "Move2");
                logWriterMove1.DoLog(false);
                logWriterMove1.DoLoggingLevels<1, 3>(false);

                WHEN("Move constructing from package")
                {
                    LoggerInterface<TestingLoggerMove> fromPackage(std::move(dataPackageMove));
                    THEN("The contents will be swaped with empty package")
                    {
                        REQUIRE(fromPackage.Str == "Move");
                        REQUIRE(fromPackage.LineNum == 32);
                        REQUIRE(dataPackageMove.Str == "");
                        REQUIRE(dataPackageMove.LineNum == 0);
                    }
                }

                WHEN("Move constructing from logWriter")
                {
                    LoggerInterface<TestingLoggerMove> moveConstructed(std::move(logWriterMove1));
                    THEN("The contents will be swaped with the other logWriter")
                    {
                        REQUIRE(moveConstructed.Str == "Move1");
                        REQUIRE(moveConstructed.LineNum == 11);
                        REQUIRE(logWriterMove1.Str == "");
                        REQUIRE(logWriterMove1.LineNum == 0);

                        REQUIRE(moveConstructed.IsLogEnabled() == false);
                        REQUIRE(moveConstructed.IsLogEnabled(1) == false);
                        REQUIRE(moveConstructed.IsLogEnabled(3) == false);

                        //The array is just copies beacuse its faster
                        // - the swapping with packages is here done only to prove that the move assignment happened
                        REQUIRE(logWriterMove1.IsLogEnabled() == false);
                        REQUIRE(logWriterMove1.IsLogEnabled(1) == false);
                        REQUIRE(logWriterMove1.IsLogEnabled(3) == false);
                    }
                }

                WHEN("Move assigning from logWriter")
                {
                    logWriterMove2 = std::move(logWriterMove1);
                    THEN("The contents will be swaped with the other logWriter")
                    {
                        REQUIRE(logWriterMove2.Str == "Move1");
                        REQUIRE(logWriterMove2.LineNum == 11);
                        REQUIRE(logWriterMove1.Str == "Move2");
                        REQUIRE(logWriterMove1.LineNum == 22);

                        REQUIRE(logWriterMove2.IsLogEnabled() == false);
                        REQUIRE(logWriterMove2.IsLogEnabled(1) == false);
                        REQUIRE(logWriterMove2.IsLogEnabled(3) == false);

                        REQUIRE(logWriterMove1.IsLogEnabled() == false);
                        REQUIRE(logWriterMove1.IsLogEnabled(1) == false);
                        REQUIRE(logWriterMove1.IsLogEnabled(3) == false);
                    }
                }
            }
        }

        TEST_CASE("Calling overloaded functions throught LoggerInterface", "[LoggerInterface]")
        {

            SECTION("Calling func: WriteMsgs", "[LoggerInterface]")
            {
                LoggerInterface<TestingLogger1> logWriter1;
                logWriter1.WriteMsgs<2>("Msg1", "Msg2");
                REQUIRE(logWriter1.LastFunctionCalled == "WriteMsgs");
            }

            SECTION("Calling func: WriteVars", "[LoggerInterface]")
            {

                LoggerInterface<TestingLogger1> logWriter1;
                logWriter1.WriteVars<2, int, int>(1, 2);
                REQUIRE(logWriter1.LastFunctionCalled == "WriteVars");
            }

            SECTION("Calling func: AppendSource", "[LoggerInterface]")
            {

                LoggerInterface<TestingLogger1> logWriter1;
                StringType file = "File";
                u32 lineNum = 32;
                //logWriter1.AppendSource(file, 32, "Value"); //Compiler error - no function overload
                logWriter1.AppendSource<2>(file, 32);
                REQUIRE(logWriter1.LastFunctionCalled == "AppendSource");
                REQUIRE(logWriter1.FileName == file);
                REQUIRE(logWriter1.LineNum == lineNum);
            }

            SECTION("Calling func: AppendTags", "[LoggerInterface]")
            {

                LoggerInterface<TestingLogger1> logWriter;
                logWriter.AppendTags<2>("tag", "tag1");
                REQUIRE(logWriter.LastFunctionCalled == "AppendTags");
            }

            /*
            SECTION("Calling func: OpenStream", "[LoggerInterface]")
            {
                LoggerInterface<TestingLogger1> logWriter;
                //logWriter.OpenStream("path", "path"); //Compiler error - no function overload
                REQUIRE(logWriter.StreamState == false);
                logWriter.OpenStream("path");
                REQUIRE(logWriter.LastFunctionCalled == "OpenStream");
                REQUIRE(logWriter.StreamState == true);
            }

            SECTION("Calling func: CloseStream", "[LoggerInterface]")
            {

                LoggerInterface<TestingLogger1> logWriter;
                REQUIRE(logWriter.StreamState == false);
                logWriter.CloseStream();
                REQUIRE(logWriter.LastFunctionCalled == "CloseStream");
                REQUIRE(logWriter.StreamState == false);
            }

            SECTION("Calling func: IsStreamOpen", "[LoggerInterface]")
            {

                LoggerInterface<TestingLogger1> logWriter;
                logWriter.IsStreamOpen();
                REQUIRE(logWriter.IsStreamOpen() == false);
                REQUIRE(logWriter.LastFunctionCalled == "IsStreamOpen");

                logWriter.OpenStream("path");
                REQUIRE(logWriter.IsStreamOpen() == true);
                logWriter.CloseStream();
                REQUIRE(logWriter.IsStreamOpen() == false);
            }

            SECTION("Calling func: FlushStream", "[LoggerInterface]")
            {
                LoggerInterface<TestingLogger1> logWriter;
                logWriter.FlushStream();
                REQUIRE(logWriter.LastFunctionCalled == "FlushStream");
            }

            SECTION("Calling func: OnConstruction", "[LoggerInterface]")
            {
                LoggerInterface<TestingLogger1> logWriter;
                //OnConstruction gets called automatically
                REQUIRE(logWriter.LastFunctionCalled == "OnConstruction");
            }

            SECTION("Calling function: OnDestruction", "[LoggerInterface]")
            {
                //Used to save the msg - the data from the logger will get destroyed
                //  along with it so its necessary to write them somwhre else
                StringType onDestructionMsg;
                {
                    LoggerInterface<TestingLogger1> logWriter;
                    logWriter.OnDestructionWriteLocation = &(onDestructionMsg);
                }
                REQUIRE(onDestructionMsg == "OnDestruction");
            }
            */

        }

        TEST_CASE("Enabling and disabling logging LoggerInterface", "[LoggerInterface]")
        {
            SECTION("IsLevelInRange", "[LoggerInterface]")
            {
                LoggerInterface<TestingLogger1> log;
                REQUIRE(log.IsLevelInRange(0) == true);
                REQUIRE(log.IsLevelInRange(log.LevelCount) == false);
                REQUIRE(log.IsLevelInRange(log.LevelCount - 1) == true);
            }
            SECTION("IsLogEnabled", "[LoggerInterface]")
            {
                LoggerInterface<TestingLogger1> log;
                REQUIRE(log.IsLogEnabled() == true);
                log.DoLog(false);
                REQUIRE(log.IsLogEnabled() == false);
            }
            SECTION("Setting Log status", "[LoggerInterface]")
            {
                LoggerInterface<TestingLogger1> log;
                REQUIRE(log.IsLogEnabled() == true);
                log.DoLog(false);
                REQUIRE(log.IsLogEnabled() == false);
                log.DoLog(true);
                REQUIRE(log.IsLogEnabled() == true);
            }
            SECTION("Enabling Log", "[LoggerInterface]")
            {
                LoggerInterface<TestingLogger1> log;
                REQUIRE(log.IsLogEnabled() == true);
                log.DoLog(false);
                REQUIRE(log.IsLogEnabled() == false);
                log.EnableLog();
                REQUIRE(log.IsLogEnabled() == true);
                log.EnableLog();
                REQUIRE(log.IsLogEnabled() == true);
            }
            SECTION("Disabling log", "[LoggerInterface]")
            {
                LoggerInterface<TestingLogger1> log;
                REQUIRE(log.IsLogEnabled() == true);
                log.DisableLog();
                REQUIRE(log.IsLogEnabled() == false);
                log.DisableLog();
                REQUIRE(log.IsLogEnabled() == false);
            }

            SECTION("Getting and Setting Log levels using function arguments", "[LoggerInterface]")
            {
                LoggerInterface<TestingLogger1> log;

                //All should be on by default
                for (auto i = log.LevelCount; i-- > 0; )
                    REQUIRE(log.IsLogEnabled(i) == true);

                //All are impacted by the DoLog function
                log.DisableLog();
                for (auto i = log.LevelCount; i-- > 0; )
                    REQUIRE(log.IsLogEnabled(i) == false);

                //Global switch should beat individual levels
                log.DoLoggingLevels(true, 1, 2, 3, 4);
                for (auto i = log.LevelCount; i-- > 0; )
                    REQUIRE(log.IsLogEnabled(i) == false);

                log.EnableLog();

                //Setting and multiple of the same argument
                log.DoLoggingLevels(false, 0, 2, 3, 3);
                REQUIRE(log.IsLogEnabled(0) == false);
                REQUIRE(log.IsLogEnabled(2) == false);
                REQUIRE(log.IsLogEnabled(3) == false);

                log.DoLoggingLevels(true, 0, 2, 3, 3);
                REQUIRE(log.IsLogEnabled(0) == true);
                REQUIRE(log.IsLogEnabled(2) == true);
                REQUIRE(log.IsLogEnabled(3) == true);

                //Order shouldnt matter
                log.DoLoggingLevels(false, 3, 2, 0, 0);
                REQUIRE(log.IsLogEnabled(0) == false);
                REQUIRE(log.IsLogEnabled(2) == false);
                REQUIRE(log.IsLogEnabled(3) == false);

                //Switching the global logging status shoudlnt reset the levels
                log.DisableLog();
                log.EnableLog();
                REQUIRE(log.IsLogEnabled(0) == false);
                REQUIRE(log.IsLogEnabled(2) == false);
                REQUIRE(log.IsLogEnabled(3) == false);

                //Functions should throw if provided with invalid argumnts
                REQUIRE_THROWS(log.DoLoggingLevels(true, 3, 2, log.MaxLevelIndex + 1, 0));
                //Non invalid levels SHOULD NOT be set - read explanation under LoggerInterface<>::DoLoggingLevels
                REQUIRE(log.IsLogEnabled(0) == false);
                REQUIRE(log.IsLogEnabled(2) == false);
                REQUIRE(log.IsLogEnabled(3) == false);
            }
            SECTION("Getting and Setting Log levels using templates", "[LoggerInterface]")
            {
                LoggerInterface<TestingLogger1> log;

                //All should be on by default
                REQUIRE(log.IsLogEnabled<0>() == true);
                REQUIRE(log.IsLogEnabled<1>() == true);
                REQUIRE(log.IsLogEnabled<2>() == true);
                REQUIRE(log.IsLogEnabled<3>() == true);
                REQUIRE(log.IsLogEnabled<4>() == true);
                //...
                REQUIRE(log.IsLogEnabled<log.MaxLevelIndex>() == true);

                //All are impacted by the DoLog function
                log.DisableLog();
                REQUIRE(log.IsLogEnabled<0>() == false);
                REQUIRE(log.IsLogEnabled<1>() == false);
                REQUIRE(log.IsLogEnabled<2>() == false);
                REQUIRE(log.IsLogEnabled<3>() == false);
                REQUIRE(log.IsLogEnabled<4>() == false);
                //...
                REQUIRE(log.IsLogEnabled<log.MaxLevelIndex>() == false);

                //Global switch should beat individual levels
                log.DoLoggingLevels(true, 1, 2, 3, 4);
                REQUIRE(log.IsLogEnabled<0>() == false);
                REQUIRE(log.IsLogEnabled<1>() == false);
                REQUIRE(log.IsLogEnabled<2>() == false);
                REQUIRE(log.IsLogEnabled<3>() == false);
                REQUIRE(log.IsLogEnabled<4>() == false);
                //...
                REQUIRE(log.IsLogEnabled<log.MaxLevelIndex>() == false);

                log.EnableLog();

                //Setting and setting multiple of the same argument
                log.DoLoggingLevels<0, 2, 3, 3>(false);
                REQUIRE(log.IsLogEnabled<0>() == false);
                REQUIRE(log.IsLogEnabled<2>() == false);
                REQUIRE(log.IsLogEnabled<3>() == false);

                log.DoLoggingLevels<0, 2, 3, 3>(true);
                REQUIRE(log.IsLogEnabled<0>() == true);
                REQUIRE(log.IsLogEnabled<2>() == true);
                REQUIRE(log.IsLogEnabled<3>() == true);

                //Order shouldnt matter
                log.DoLoggingLevels<3, 2, 0, 0>(false);
                REQUIRE(log.IsLogEnabled<0>() == false);
                REQUIRE(log.IsLogEnabled<2>() == false);
                REQUIRE(log.IsLogEnabled<3>() == false);


                //Switching the global logging status shoudlnt reset the levels
                log.DisableLog();
                log.EnableLog();
                REQUIRE(log.IsLogEnabled<0>() == false);
                REQUIRE(log.IsLogEnabled<2>() == false);
                REQUIRE(log.IsLogEnabled<3>() == false);

                //Functions should not compile if provided with invalid argumnts
                //log.DoLoggingLevels<3, 2, log.MaxLevelIndex + 1, 0>(true);
            }
        }

    }

    namespace DefaultDataInterpretTesting
    {

        template<typename T, typename valType>
        void TestConversion(valType val, std::string PASS_REF output, DefaultDataInterpret PASS_REF interpret, std::string expectedResult)
        {
            interpret.InterpretArg(static_cast<T>(val), output);
            REQUIRE(output == expectedResult);
        }

        TEST_CASE("Conversions", "[DefaultDataInterpret]")
        {
            DefaultDataInterpret interpret;
            std::string output;
            output.reserve(512);

            SECTION("Converting from int")
            {
                TestConversion<int>(132, output, interpret, "132");
                TestConversion<int>(-132, output, interpret, "-132");
                TestConversion<int>(0.3, output, interpret, "0");
                TestConversion<int>(-0.3, output, interpret, "0");
            }

            SECTION("Converting from long")
            {
                TestConversion<long>(132, output, interpret, "132");
                TestConversion<long>(-132, output, interpret, "-132");
                TestConversion<long>(0.3, output, interpret, "0");
                TestConversion<long>(-0.3, output, interpret, "0");
            }

            SECTION("Converting from long long")
            {
                TestConversion<long long>(132, output, interpret, "132");
                TestConversion<long long>(-132, output, interpret, "-132");
                TestConversion<long long>(0.3, output, interpret, "0");
                TestConversion<long long>(-0.3, output, interpret, "0");
            }

            SECTION("Converting from unsigned")
            {
                TestConversion<unsigned>(132, output, interpret, "132");
                TestConversion<unsigned>(0.3, output, interpret, "0");
                TestConversion<unsigned>(-0.3, output, interpret, "0");
            }

            SECTION("Converting from unsigned long")
            {
                TestConversion<unsigned long>(132, output, interpret, "132");
                TestConversion<unsigned long>(0.3, output, interpret, "0");
                TestConversion<unsigned long>(-0.3, output, interpret, "0");
            }

            SECTION("Converting from unsigned long long")
            {
                TestConversion<unsigned long long>(132, output, interpret, "132");
                TestConversion<unsigned long long>(0.3, output, interpret, "0");
                TestConversion<unsigned long long>(-0.3, output, interpret, "0");
            }

            SECTION("Converting from float")
            {
                TestConversion<float>(132, output, interpret, "132.000000");
                TestConversion<float>(-132, output, interpret, "-132.000000");
                TestConversion<float>(0.3, output, interpret, "0.300000");
                TestConversion<float>(-0.3, output, interpret, "-0.300000");
            }

            SECTION("Converting from double")
            {
                TestConversion<double>(132, output, interpret, "132.000000");
                TestConversion<double>(-132, output, interpret, "-132.000000");
                TestConversion<double>(0.3, output, interpret, "0.300000");
                TestConversion<double>(-0.3, output, interpret, "-0.300000");
            }

            SECTION("Converting from long double")
            {
                TestConversion<long double>(132, output, interpret, "132.0000000000000000");
                TestConversion<long double>(-132, output, interpret, "-132.0000000000000000");
                TestConversion<long double>(0.3, output, interpret, "0.3000000000000000");
                TestConversion<long double>(-0.3, output, interpret, "-0.3000000000000000");
            }

            SECTION("Converting from pointer")
            {
                void POINTER ptr;
                ptr = reinterpret_cast<void POINTER>(0x0);
                TestConversion<void POINTER>(ptr, output, interpret, "0x0");
                ptr = reinterpret_cast<void POINTER>(0xf);
                TestConversion<void POINTER>(ptr, output, interpret, "0xf");
                ptr = reinterpret_cast<void POINTER>(0xf00f);
                TestConversion<void POINTER>(ptr, output, interpret, "0xf00f");
            }

            SECTION("Converting from bool")
            {
                TestConversion<bool>(true, output, interpret, "true");
                TestConversion<bool>(false, output, interpret, "false");
            }

            SECTION("Converting from char *")
            {
                const char * myCStr1 = "cstr1";
                const char * myCStr2 = "CSTR2";
                const char * myCStr3 = "";
                TestConversion<const char *>(myCStr1, output, interpret, "cstr1");
                TestConversion<const char *>(myCStr2, output, interpret, "CSTR2");
                TestConversion<const char *>(myCStr3, output, interpret, "");
            }

            SECTION("Converting from std::string")
            {
                const std::string myStr1 = "str1";
                const std::string myStr2 = "STR2";
                const std::string myStr3 = "";
                TestConversion<std::string>(myStr1, output, interpret, "str1");
                TestConversion<std::string>(myStr2, output, interpret, "STR2");
                TestConversion<std::string>(myStr3, output, interpret, "");
            }

            SECTION("Converting from std::string_view")
            {
                TestConversion<std::string_view>("str1", output, interpret, "str1");
                TestConversion<std::string_view>("STR2", output, interpret, "STR2");
                TestConversion<std::string_view>("", output, interpret, "");
            }
        }
    }

    namespace DefaultLogWritterPackageTesting
    {
        TEST_CASE("DefaultLogWritterPackage Solo Construction and Assignment", "[DefaultLogWritterPackage]")
        {
            DefaultLoggerPackageTester Tester1("My file");
            DefaultLoggerPackageTester Tester2("My file");

            SECTION("Default construction member inicialisation")
            {
                REQUIRE(Tester1.Formater.size() == 0);
                REQUIRE(Tester1.CollectionString.capacity() >= Tester1.MinimumCollectionStringSize);
                REQUIRE(Tester1.TempString.capacity() >= Tester1.MinimumTempStringSize);
                REQUIRE(Tester1.Messages.capacity() >= Tester1.MinimumMessagesSize);
                REQUIRE(Tester1.Variables.capacity() >= Tester1.MinimumVariablesSize);
                REQUIRE(Tester1.Tags.capacity() >= Tester1.MinimumTagsSize);
                REQUIRE(Tester1.Source.capacity() >= Tester1.MinimumSourceSize);
                REQUIRE(Tester1.EntryIndex == 0);
            }

            Tester1.CollectionString = "Collection string";
            Tester1.TempString = "Temp string";
            Tester1.Messages = "Messages";
            Tester1.Variables = "Variables";
            Tester1.Tags = "Tags";
            Tester1.Source = "Source";
            Tester1.EntryIndex = 1;

            SECTION("Move assignment")
            {
                Tester2 = std::move(Tester1);
                REQUIRE(Tester2.CollectionString == "Collection string");
                REQUIRE(Tester2.TempString == "Temp string");
                REQUIRE(Tester2.Messages == "Messages");
                REQUIRE(Tester2.Variables == "Variables");
                REQUIRE(Tester2.Tags == "Tags");
                REQUIRE(Tester2.Source == "Source");
                REQUIRE(Tester2.EntryIndex == 1);

                REQUIRE(Tester1.CollectionString == "");
                REQUIRE(Tester1.TempString == "");
            }

            SECTION("Move constructor")
            {
                DefaultLoggerPackageTester Tester3(std::move(Tester1));
                REQUIRE(Tester3.CollectionString == "Collection string");
                REQUIRE(Tester3.TempString == "Temp string");
                REQUIRE(Tester3.Messages == "Messages");
                REQUIRE(Tester3.Variables == "Variables");
                REQUIRE(Tester3.Tags == "Tags");
                REQUIRE(Tester3.Source == "Source");
                REQUIRE(Tester3.EntryIndex == 1);

                REQUIRE(Tester2.CollectionString == "");
                REQUIRE(Tester2.TempString == "");
            }
        }

        TEST_CASE("Construction DefaultLogWritterPackageTesting inside LoggerInterface", "[DefaultLogWritter]")
        {
            GIVEN("Two logWriters with move constructors and assignments")
            {
                DefaultLoggerPackage dataPackage("FilePackage");
                GetDefPackageTester(dataPackage).CollectionString   = "PackageCollectionString";
                GetDefPackageTester(dataPackage).TempString         = "PackageTempString";
                GetDefPackageTester(dataPackage).Messages           = "PackageMessages";
                GetDefPackageTester(dataPackage).Variables          = "PackageVariables";
                GetDefPackageTester(dataPackage).Tags               = "PackageTags";
                GetDefPackageTester(dataPackage).Source             = "PackageSource";
                GetDefPackageTester(dataPackage).EntryIndex         = 33;

                LoggerInterface<DefaultLogger> logWriter1("File1");
                LoggerInterface<DefaultLogger> logWriter2("File2");

                GetDefLoggerTester(logWriter1).CollectionString   = "Logger1CollectionString";
                GetDefLoggerTester(logWriter1).TempString         = "Logger1TempString";
                GetDefLoggerTester(logWriter1).Messages           = "Logger1Messages";
                GetDefLoggerTester(logWriter1).Variables          = "Logger1Variables";
                GetDefLoggerTester(logWriter1).Tags               = "Logger1Tags";
                GetDefLoggerTester(logWriter1).Source             = "Logger1Source";
                GetDefLoggerTester(logWriter1).EntryIndex         = 11;

                logWriter1.DoLog(false);
                logWriter1.DoLoggingLevels<1, 3>(false);

                WHEN("Move constructing from package")
                {
                    LoggerInterface<DefaultLogger> fromPackage(std::move(dataPackage));
                    THEN("The contents will be swaped with the package")
                    {
                        REQUIRE(GetDefLoggerTester(logWriter2).CollectionString    == "Logger1CollectionString");
                        REQUIRE(GetDefLoggerTester(logWriter2).TempString          == "Logger1TempString");
                        REQUIRE(GetDefLoggerTester(logWriter2).Messages            == "Logger1Messages");
                        REQUIRE(GetDefLoggerTester(logWriter2).Variables           == "Logger1Variables");
                        REQUIRE(GetDefLoggerTester(logWriter2).Tags                == "Logger1Tags");
                        REQUIRE(GetDefLoggerTester(logWriter2).Source              == "Logger1Source");
                        REQUIRE(GetDefLoggerTester(logWriter2).EntryIndex          == 11);

                        REQUIRE(GetDefPackageTester(dataPackage).CollectionString   == "");
                        REQUIRE(GetDefPackageTester(dataPackage).TempString         == "");
                        REQUIRE(GetDefPackageTester(dataPackage).Messages           == "");
                        REQUIRE(GetDefPackageTester(dataPackage).Variables          == "");
                        REQUIRE(GetDefPackageTester(dataPackage).Tags               == "");
                        REQUIRE(GetDefPackageTester(dataPackage).Source             == "");
                        REQUIRE(GetDefPackageTester(dataPackage).EntryIndex         == 33); //trivially constructible types are just copied
                    }
                }

                WHEN("Move constructing from logWriter")
                {
                    LoggerInterface<DefaultLogger> moveConstructed(std::move(logWriter1));
                    THEN("The contents will be swaped with the logger")
                    {
                        REQUIRE(GetDefLoggerTester(logWriter2).CollectionString    == "Logger1CollectionString");
                        REQUIRE(GetDefLoggerTester(logWriter2).TempString          == "Logger1TempString");
                        REQUIRE(GetDefLoggerTester(logWriter2).Messages            == "Logger1Messages");
                        REQUIRE(GetDefLoggerTester(logWriter2).Variables           == "Logger1Variables");
                        REQUIRE(GetDefLoggerTester(logWriter2).Tags                == "Logger1Tags");
                        REQUIRE(GetDefLoggerTester(logWriter2).Source              == "Logger1Source");
                        REQUIRE(GetDefLoggerTester(logWriter2).EntryIndex          == 11);

                        REQUIRE(GetDefLoggerTester(logWriter1).CollectionString    == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).TempString          == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Messages            == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Variables           == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Tags                == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Source              == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).EntryIndex          == 11); //trivially constructible types are just copied

                        //Status level array is just copied
                        REQUIRE(moveConstructed.IsLogEnabled() == false);
                        REQUIRE(moveConstructed.IsLogEnabled(1) == false);
                        REQUIRE(moveConstructed.IsLogEnabled(3) == false);

                        REQUIRE(logWriter1.IsLogEnabled() == false);
                        REQUIRE(logWriter1.IsLogEnabled(1) == false);
                        REQUIRE(logWriter1.IsLogEnabled(3) == false);
                    }
                }

                WHEN("Move assigning from logWriter")
                {
                    logWriter2 = std::move(logWriter1);
                    THEN("The contents will be swaped or set equal to the other logWriter")
                    {
                        //No OnConstruction function is called when move assigning therefore the values are either swapped or set equal
                        REQUIRE(GetDefLoggerTester(logWriter2).CollectionString    == "Logger1CollectionString");
                        REQUIRE(GetDefLoggerTester(logWriter2).TempString          == "Logger1TempString");
                        REQUIRE(GetDefLoggerTester(logWriter2).Messages            == "Logger1Messages");
                        REQUIRE(GetDefLoggerTester(logWriter2).Variables           == "Logger1Variables");
                        REQUIRE(GetDefLoggerTester(logWriter2).Tags                == "Logger1Tags");
                        REQUIRE(GetDefLoggerTester(logWriter2).Source              == "Logger1Source");
                        REQUIRE(GetDefLoggerTester(logWriter2).EntryIndex          == 11);

                        REQUIRE(GetDefLoggerTester(logWriter1).CollectionString    == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).TempString          == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Messages            == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Variables           == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Tags                == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Source              == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).EntryIndex          == 11); //trivially constructible types are just copied

                        //Status level array is just copied
                        REQUIRE(logWriter2.IsLogEnabled() == false);
                        REQUIRE(logWriter2.IsLogEnabled(1) == false);
                        REQUIRE(logWriter2.IsLogEnabled(3) == false);

                        REQUIRE(logWriter1.IsLogEnabled() == false);
                        REQUIRE(logWriter1.IsLogEnabled(1) == false);
                        REQUIRE(logWriter1.IsLogEnabled(3) == false);
                    }
                }
            }
        }
    }

    namespace DefaultLogWritterTesting
    {
        TEST_CASE("Construction DefaultLogWritter", "[DefaultLogWritter]")
        {
            GIVEN("Two logWriters with move constructors and assignments")
            {
                DefaultLoggerPackage dataPackage("FilePackage");
                GetDefPackageTester(dataPackage).CollectionString   = "PackageCollectionString";
                GetDefPackageTester(dataPackage).TempString         = "PackageTempString";
                GetDefPackageTester(dataPackage).Messages           = "PackageMessages";
                GetDefPackageTester(dataPackage).Variables          = "PackageVariables";
                GetDefPackageTester(dataPackage).Tags               = "PackageTags";
                GetDefPackageTester(dataPackage).Source             = "PackageSource";
                GetDefPackageTester(dataPackage).EntryIndex         = 33;

                DefaultLogger logWriter1("File1");
                DefaultLogger logWriter2("File2");

                GetDefLoggerTester(logWriter1).CollectionString   = "Logger1CollectionString";
                GetDefLoggerTester(logWriter1).TempString         = "Logger1TempString";
                GetDefLoggerTester(logWriter1).Messages           = "Logger1Messages";
                GetDefLoggerTester(logWriter1).Variables          = "Logger1Variables";
                GetDefLoggerTester(logWriter1).Tags               = "Logger1Tags";
                GetDefLoggerTester(logWriter1).Source             = "Logger1Source";
                GetDefLoggerTester(logWriter1).EntryIndex         = 11;

                logWriter1.DoLog(false);
                logWriter1.DoLoggingLevels<1, 3>(false);

                WHEN("Move constructing from package")
                {
                    DefaultLogger fromPackage(std::move(dataPackage));
                    THEN("The contents will be swaped with empty package but then reset as a result of OnConstruction")
                    {
                        //All strings except TempSTring get reset as result of pushing out the OnConstruction message
                        REQUIRE(GetDefLoggerTester(fromPackage).CollectionString    == "");
                        //REQUIRE(GetDefLoggerTester(fromPackage).TempString          == "");
                        REQUIRE(GetDefLoggerTester(fromPackage).Messages            == "");
                        REQUIRE(GetDefLoggerTester(fromPackage).Variables           == "");
                        REQUIRE(GetDefLoggerTester(fromPackage).Tags                == "");
                        REQUIRE(GetDefLoggerTester(fromPackage).Source              == "");
                        REQUIRE(GetDefLoggerTester(fromPackage).EntryIndex          == 34); //1 is added cause of the OnConstruction msg

                        REQUIRE(GetDefPackageTester(dataPackage).CollectionString   == "");
                        REQUIRE(GetDefPackageTester(dataPackage).TempString         == "");
                        REQUIRE(GetDefPackageTester(dataPackage).Messages           == "");
                        REQUIRE(GetDefPackageTester(dataPackage).Variables          == "");
                        REQUIRE(GetDefPackageTester(dataPackage).Tags               == "");
                        REQUIRE(GetDefPackageTester(dataPackage).Source             == "");
                        REQUIRE(GetDefPackageTester(dataPackage).EntryIndex         == 33); //trivially constructible types are just copied
                    }
                }

                WHEN("Move constructing from logWriter")
                {
                    DefaultLogger moveConstructed(std::move(logWriter1));
                    THEN("The contents will be swaped with empty package but then reset as a result of OnConstruction")
                    {
                        //All strings except TempSTring get reset as result of pushing out the OnConstruction message
                        REQUIRE(GetDefLoggerTester(moveConstructed).CollectionString    == "");
                        //REQUIRE(GetDefLoggerTester(moveConstructed).TempString          == "");
                        REQUIRE(GetDefLoggerTester(moveConstructed).Messages            == "");
                        REQUIRE(GetDefLoggerTester(moveConstructed).Variables           == "");
                        REQUIRE(GetDefLoggerTester(moveConstructed).Tags                == "");
                        REQUIRE(GetDefLoggerTester(moveConstructed).Source              == "");
                        REQUIRE(GetDefLoggerTester(moveConstructed).EntryIndex          == 12); //1 is added cause of the OnConstruction msg

                        REQUIRE(GetDefLoggerTester(logWriter1).CollectionString    == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).TempString          == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Messages            == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Variables           == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Tags                == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Source              == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).EntryIndex          == 11); //trivially constructible types are just copied

                        //Status level array is just copied
                        REQUIRE(moveConstructed.IsLogEnabled() == false);
                        REQUIRE(moveConstructed.IsLogEnabled(1) == false);
                        REQUIRE(moveConstructed.IsLogEnabled(3) == false);

                        REQUIRE(logWriter1.IsLogEnabled() == false);
                        REQUIRE(logWriter1.IsLogEnabled(1) == false);
                        REQUIRE(logWriter1.IsLogEnabled(3) == false);
                    }
                }

                WHEN("Move assigning from logWriter")
                {
                    logWriter2 = std::move(logWriter1);
                    THEN("The contents will be swaped or set equal to the other logWriter")
                    {
                        //No OnConstruction function is called when move assigning therefore the values are either swapped or set equal
                        REQUIRE(GetDefLoggerTester(logWriter2).CollectionString    == "Logger1CollectionString");
                        REQUIRE(GetDefLoggerTester(logWriter2).TempString          == "Logger1TempString");
                        REQUIRE(GetDefLoggerTester(logWriter2).Messages            == "Logger1Messages");
                        REQUIRE(GetDefLoggerTester(logWriter2).Variables           == "Logger1Variables");
                        REQUIRE(GetDefLoggerTester(logWriter2).Tags                == "Logger1Tags");
                        REQUIRE(GetDefLoggerTester(logWriter2).Source              == "Logger1Source");
                        REQUIRE(GetDefLoggerTester(logWriter2).EntryIndex          == 11);

                        REQUIRE(GetDefLoggerTester(logWriter1).CollectionString    == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).TempString          == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Messages            == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Variables           == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Tags                == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).Source              == "");
                        REQUIRE(GetDefLoggerTester(logWriter1).EntryIndex          == 11); //trivially constructible types are just copied

                        //Status level array is just copied
                        REQUIRE(logWriter2.IsLogEnabled() == false);
                        REQUIRE(logWriter2.IsLogEnabled(1) == false);
                        REQUIRE(logWriter2.IsLogEnabled(3) == false);

                        REQUIRE(logWriter1.IsLogEnabled() == false);
                        REQUIRE(logWriter1.IsLogEnabled(1) == false);
                        REQUIRE(logWriter1.IsLogEnabled(3) == false);
                    }
                }
            }
        }

        TEST_CASE("Using constant adding functions", "[DefaultLogWritter]")
        {
            GIVEN("A LogWriter and a output string and formater")
            {
                DefaultLogger writer("File1");
                std::string output;
                fmt::format_int formater;
                output.reserve(256);

                SECTION("ResetString")
                {
                    GetDefLoggerTester(writer).ResetString(output);
                    REQUIRE(output.empty() == true);
                    output = "Hello";
                    GetDefLoggerTester(writer).ResetString(output);
                    REQUIRE(output.empty() == true);
                }

                SECTION("PutNumIntoCharcterField")
                {
                    GetDefLoggerTester(writer).PutNumIntoCharcterField(32, 8, formater, output);
                    REQUIRE(output == "00000032");
                    GetDefLoggerTester(writer).PutNumIntoCharcterField(1111, 8, formater, output);
                    REQUIRE(output == "0000003200001111");
                    output.clear();

                    GetDefLoggerTester(writer).PutNumIntoCharcterField(1111, 3, formater, output);
                    REQUIRE(output == "1111");
                    output.clear();

                    GetDefLoggerTester(writer).PutNumIntoCharcterField(1111, 0, formater, output);
                    REQUIRE(output == "1111");
                    output.clear();

                    GetDefLoggerTester(writer).PutNumIntoCharcterField(0, 8, formater, output);
                    REQUIRE(output == "00000000");
                    output.clear();
                }

                SECTION("AddNewline")
                {
                    GetDefLoggerTester(writer).AddNewline(output);
                    REQUIRE(output[0] == '\n');
                    GetDefLoggerTester(writer).AddNewline(output);
                    GetDefLoggerTester(writer).AddNewline(output);
                    REQUIRE(output[0] == '\n');
                    REQUIRE(output[2] == '\n');
                }

                SECTION("AddSeparator")
                {
                    constexpr char separator = DefaultLoggerTester::Separator;
                    GetDefLoggerTester(writer).AddSeparator(output);
                    REQUIRE(output[0] == separator);
                    GetDefLoggerTester(writer).AddSeparator(output);
                    GetDefLoggerTester(writer).AddSeparator(output);
                    REQUIRE(output[0] == separator);
                    REQUIRE(output[2] == separator);
                }

                SECTION("AddFormatedVariable")
                {
                    GetDefLoggerTester(writer).AddFormatedVariable("Name", "Value", output);
                    REQUIRE(output == "Name = Value");
                    GetDefLoggerTester(writer).AddFormatedVariable("Name2", "Value2", output);
                    REQUIRE(output == "Name = ValueName2 = Value2");
                    output.clear();
                    GetDefLoggerTester(writer).AddFormatedVariable("32", "64", output);
                    REQUIRE(output == "32 = 64");
                    output.clear();
                    GetDefLoggerTester(writer).AddFormatedVariable("\n", "\t", output);
                    REQUIRE(output == "\n = \t");
                }

                SECTION("AddFormatedTag")
                {
                    GetDefLoggerTester(writer).AddFormatedTag("tag", output);
                    REQUIRE(output == "<tag>");
                    GetDefLoggerTester(writer).AddFormatedTag("tag2", output);
                    REQUIRE(output == "<tag><tag2>");
                    output.clear();
                    GetDefLoggerTester(writer).AddFormatedTag("\n", output);
                    REQUIRE(output == "<\n>");
                }

                SECTION("AddUnformatedMsgPart")
                {
                    GetDefLoggerTester(writer).AddUnformatedMsgPart("msg", output);
                    REQUIRE(output == "msg");
                    GetDefLoggerTester(writer).AddUnformatedMsgPart("msg2", output);
                    REQUIRE(output == "msgmsg2");
                    output.clear();
                    GetDefLoggerTester(writer).AddUnformatedMsgPart("\n", output);
                    REQUIRE(output == "\n");
                }

                SECTION("AddFormatedMsg")
                {
                    GetDefLoggerTester(writer).AddFormatedMsg("msg", output);
                    REQUIRE(output == "msg = msg");
                    GetDefLoggerTester(writer).AddFormatedMsg("msg2", output);
                    REQUIRE(output == "msg = msgmsg = msg2");
                    output.clear();
                    GetDefLoggerTester(writer).AddFormatedMsg("\n", output);
                    REQUIRE(output == "msg = \n");
                }

                SECTION("AddFormatedLevel")
                {
                    GetDefLoggerTester(writer).AddFormatedLevel(15, formater, output);
                    REQUIRE(output == "<15>");
                    GetDefLoggerTester(writer).AddFormatedLevel(3, formater, output);
                    REQUIRE(output == "<15><03>");
                    output.clear();
                    GetDefLoggerTester(writer).AddFormatedLevel(0, formater, output);
                    REQUIRE(output == "<00>");
                }

                SECTION("AddFormatedIterationCount")
                {
                    GetDefLoggerTester(writer).AddFormatedIterationCount(32, formater, output);
                    REQUIRE(output == "00000032");
                    GetDefLoggerTester(writer).AddFormatedIterationCount(64, formater, output);
                    REQUIRE(output == "0000003200000064");
                    output.clear();
                    GetDefLoggerTester(writer).AddFormatedIterationCount(0, formater, output);
                    REQUIRE(output == "00000000");
                }

                SECTION("AddFormatedSource")
                {
                    GetDefLoggerTester(writer).AddFormatedSource("file1", 32, formater, output);
                    REQUIRE(output == "file1 [32]");
                    GetDefLoggerTester(writer).AddFormatedSource("file2", 64, formater, output);
                    REQUIRE(output == "file1 [32]file2 [64]");
                    output.clear();
                    GetDefLoggerTester(writer).AddFormatedSource("\n", 0, formater, output);
                    REQUIRE(output == "\n [0]");
                }

                SECTION("AddFormatedTime")
                {
                    time_t rawTime;
                    time(&rawTime);
                    unsigned long long lenght;
                    GetDefLoggerTester(writer).AddFormatedTime(formater, rawTime, clock(), output);
                    REQUIRE(output.empty() == false);
                    lenght = output.size();
                    GetDefLoggerTester(writer).AddFormatedTime(formater, rawTime, clock(), output);
                    REQUIRE(output.empty() == false);
                    REQUIRE(output.size() > lenght);
                    output.clear();

                    GetDefLoggerTester(writer).AddFormatedTime(formater, 0, 0, output);
                    REQUIRE(output.empty() == false);
                    lenght = output.size();
                    output.clear();

                    GetDefLoggerTester(writer).AddFormatedTime(formater, std::numeric_limits<unsigned long long>::max(), std::numeric_limits<unsigned long>::max(), output);
                    REQUIRE(output.empty() == false);
                    lenght = output.size();
                }

            }
        }

        TEST_CASE("Using UnravelAndAddMsgs", "[DefaultLogWritter]")
        {
            GIVEN("A LogWriter")
            {
                DefaultLogger writer("File1");
                std::string expected;
                std::string temp;
                expected.reserve(256);
                temp.reserve(256);

                WHEN("Is given just strings")
                {
                    GetDefLoggerTester(writer).UnravelAndAddMsgs("msg1", "msg2", "msg3", "msg4");
                    THEN("Concatenates them together")
                    {
                        REQUIRE(GetDefLoggerTester(writer).Messages == "msg1msg2msg3msg4");
                    }
                }

                WHEN("Is given strings numbers and pointers")
                {
                    GetDefLoggerTester(writer).UnravelAndAddMsgs("Num: ", 32, "; Address: ", ADDRESS(expected));
                    THEN("Concatenates them together after conversion from Interpret")
                    {
                        expected = "Num: 32; Address: ";
                        GetDefLoggerTester(writer).Interpret.InterpretArg(ADDRESS(expected), temp);
                        expected += temp;
                        REQUIRE(GetDefLoggerTester(writer).Messages == expected);
                    }
                }

                WHEN("Is used several times consequently")
                {
                    GetDefLoggerTester(writer).UnravelAndAddMsgs("msg1", "msg2", "msg3", "msg4");
                    GetDefLoggerTester(writer).UnravelAndAddMsgs("msg5");
                    GetDefLoggerTester(writer).UnravelAndAddMsgs("msg6", "msg7");
                    THEN("Adds the messeges accross all of the calls")
                    {
                        REQUIRE(GetDefLoggerTester(writer).Messages == "msg1msg2msg3msg4msg5msg6msg7");
                    }
                }

                WHEN("Is given no arguments")
                {
                    GetDefLoggerTester(writer).Messages = "Preset";
                    GetDefLoggerTester(writer).UnravelAndAddMsgs();
                    THEN("Does nothing")
                    {
                        REQUIRE(GetDefLoggerTester(writer).Messages == "Preset");
                    }
                }
            }
        }

        TEST_CASE("Using UnravelAndAddVariables", "[DefaultLogWritter]")
        {
            GIVEN("A LogWriter")
            {
                DefaultLogger writer("File1");
                std::string expected;
                std::string temp;
                expected.reserve(256);
                temp.reserve(256);

                WHEN("Is given a simple variable")
                {
                    GetDefLoggerTester(writer).UnravelAndAddVariables("varName", 0);
                    THEN("Concatenates them together in formated form")
                    {
                        REQUIRE(GetDefLoggerTester(writer).Variables == "varName = 0 ");
                    }
                }

                WHEN("Is given strings numbers and pointers")
                {
                    GetDefLoggerTester(writer).UnravelAndAddVariables("var1", "value", "var2", 32, "var3", ADDRESS(expected));
                    THEN("Concatenates them together after conversion from Interpret")
                    {
                        expected = "var1 = value var2 = 32 var3 = ";
                        GetDefLoggerTester(writer).Interpret.InterpretArg(ADDRESS(expected), temp);
                        expected += temp;
                        expected += " ";
                        REQUIRE(GetDefLoggerTester(writer).Variables == expected);
                    }
                }

                WHEN("Is used several times consequently")
                {
                    GetDefLoggerTester(writer).UnravelAndAddVariables<StringType, StringType, StringType, StringType>("var", "value", "var", "value");
                    GetDefLoggerTester(writer).UnravelAndAddVariables<StringType, StringType>("var", "value");
                    THEN("Adds the variable messeges accross all of the calls")
                    {
                        REQUIRE(GetDefLoggerTester(writer).Variables == "var = value var = value var = value ");
                    }
                }

                WHEN("Is given no arguments")
                {
                    GetDefLoggerTester(writer).Variables = "Preset";
                    GetDefLoggerTester(writer).UnravelAndAddVariables();
                    THEN("Does nothing")
                    {
                        REQUIRE(GetDefLoggerTester(writer).Variables == "Preset");
                    }
                }
            }
        }

        TEST_CASE("Using UnravelAndAppendTags", "[DefaultLogWritter]")
        {
            GIVEN("A LogWriter")
            {
                DefaultLogger writer("File1");

                WHEN("Is given just strings")
                {
                    GetDefLoggerTester(writer).UnravelAndAddTags("tag1", "tag2", "tag3", "tag4");
                    THEN("Concatenates them together in formated manner")
                    {
                        REQUIRE(GetDefLoggerTester(writer).Tags == "<tag1> <tag2> <tag3> <tag4> ");
                    }
                }
                WHEN("Is used several times consequently")
                {
                    GetDefLoggerTester(writer).UnravelAndAddTags("tag1", "tag2", "tag3", "tag4");
                    GetDefLoggerTester(writer).UnravelAndAddTags("tag1");
                    GetDefLoggerTester(writer).UnravelAndAddTags("tag1", "tag2");
                    THEN("Adds the tags accross all of the calls")
                    {
                        REQUIRE(GetDefLoggerTester(writer).Tags == "<tag1> <tag2> <tag3> <tag4> <tag1> <tag1> <tag2> ");
                    }
                }

                WHEN("Is given no arguments")
                {
                    GetDefLoggerTester(writer).Tags = "Preset";
                    GetDefLoggerTester(writer).UnravelAndAddTags();
                    THEN("Does nothing")
                    {
                        REQUIRE(GetDefLoggerTester(writer).Tags == "Preset");
                    }
                }
            }
        }
    }
}

#endif // STATICLOG2_H
