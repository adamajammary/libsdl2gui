#if defined _windows && defined _DEBUG

#include <windows.h>
#include <CppUnitTest.h>
#include <libsdl2gui.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LSG_UnitTest
{
    std::string WorkingDir = "";

    TEST_MODULE_INITIALIZE(Start)
    {
        try
        {
            HMODULE      hmodule;
            static TCHAR moduleName;
            TCHAR        pathA[MAX_PATH + 1];

            GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, &moduleName, &hmodule);
            GetModuleFileNameA(hmodule, pathA, (MAX_PATH + 1));

            auto path = std::string(pathA);

            WorkingDir = path.substr(0, path.rfind("\\") + 1);

            LSG_StartTest("ui/main.xml", WorkingDir);

            Assert::IsTrue(LSG_IsRunning());
        }
        catch (const std::exception& e)
        {
            Assert::Fail(ToString(e.what()).c_str());
        }
    }

    TEST_MODULE_CLEANUP(Quit)
    {
        LSG_Quit();
    }

	TEST_CLASS(List)
	{
        TEST_METHOD(AddItem)
        {
            try
            {
                for (int i = 0; i < 200; i++)
                    LSG_AddListItem("List", "My new list item number " + std::to_string(i));

                auto items     = LSG_GetListItems("List");
                auto pageItems = LSG_GetPageListItems("List");

                Assert::AreEqual(212,                   (int)items.size());
                Assert::AreEqual(LSG_MAX_ROWS_PER_PAGE, (int)pageItems.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetItem)
        {
            try
            {
                SetItems();

                auto itemN1  = LSG_GetListItem("List", -1);
                auto item0   = LSG_GetListItem("List", 0);
                auto item199 = LSG_GetListItem("List", 199);

                Assert::IsTrue(itemN1.empty());
                Assert::AreEqual("My new list item number 0",  item0.c_str());
                Assert::AreEqual("My new list item number 99", item199.c_str());

                auto pageItemN1  = LSG_GetPageListItem("List", -1);
                auto pageItem0   = LSG_GetPageListItem("List", 0);
                auto pageItem199 = LSG_GetPageListItem("List", 199);

                Assert::IsTrue(pageItemN1.empty());
                Assert::AreEqual("My new list item number 0", pageItem0.c_str());
                Assert::IsTrue(pageItem199.empty());

                LSG_SetPage("List", 1);

                item0   = LSG_GetListItem("List", 0);
                item199 = LSG_GetListItem("List", 199);

                Assert::AreEqual("My new list item number 0",  item0.c_str());
                Assert::AreEqual("My new list item number 99", item199.c_str());

                pageItem0   = LSG_GetPageListItem("List", 0);
                pageItem199 = LSG_GetPageListItem("List", 199);

                Assert::AreEqual("My new list item number 189", pageItem0.c_str());
                Assert::IsTrue(pageItem199.empty());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetItems)
        {
            try
            {
                SetItems();

                auto items     = LSG_GetListItems("List");
                auto pageItems = LSG_GetPageListItems("List");

                Assert::AreEqual(200,                   (int)items.size());
                Assert::AreEqual(LSG_MAX_ROWS_PER_PAGE, (int)pageItems.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(RemoveItem)
        {
            try
            {
                SetItems();

                LSG_RemoveListItem("List", -1);
                LSG_RemoveListItem("List", 0);
                LSG_RemoveListItem("List", 199);

                auto item0   = LSG_GetListItem("List", 0);
                auto item197 = LSG_GetListItem("List", 197);
                auto item198 = LSG_GetListItem("List", 198);
                auto item199 = LSG_GetListItem("List", 199);

                Assert::AreEqual("My new list item number 1",  item0.c_str());
                Assert::AreEqual("My new list item number 98", item197.c_str());
                Assert::AreEqual("My new list item number 99", item198.c_str());
                Assert::IsTrue(item199.empty());

                LSG_RemovePageListItem("List", -1);
                LSG_RemovePageListItem("List", 0);
                LSG_RemovePageListItem("List", 198);

                LSG_SetPage("List", 1);

                auto pageItem0 = LSG_GetPageListItem("List", 0);

                Assert::AreEqual("My new list item number 190", pageItem0.c_str());

                item0   = LSG_GetListItem("List", 0);
                item197 = LSG_GetListItem("List", 197);
                item198 = LSG_GetListItem("List", 198);
                item199 = LSG_GetListItem("List", 199);

                Assert::AreEqual("My new list item number 10", item0.c_str());
                Assert::AreEqual("My new list item number 99", item197.c_str());
                Assert::IsTrue(item198.empty());
                Assert::IsTrue(item199.empty());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetItem)
        {
            try
            {
                SetItems();

                LSG_SetListItem("List", 0,   "A - The first item!");
                LSG_SetListItem("List", 199, "Z - The last item!");

                auto item0   = LSG_GetListItem("List", 0);
                auto item199 = LSG_GetListItem("List", 199);

                Assert::AreEqual("A - The first item!", item0.c_str());
                Assert::AreEqual("Z - The last item!",  item199.c_str());

                LSG_SetPageListItem("List", 0,   "A - The first page item!");
                LSG_SetPageListItem("List", 199, "Z - The last page item!");

                auto pageItem0   = LSG_GetPageListItem("List", 0);
                auto pageItem199 = LSG_GetPageListItem("List", 199);

                Assert::IsTrue(pageItem199.empty());

                Assert::AreEqual("A - The first page item!", pageItem0.c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetItems)
        {
            try
            {
                LSG_Strings newItems = {};

                for (int i = 0; i < 200; i++)
                    newItems.push_back("My new list item number " + std::to_string(i));

                LSG_SetPage("List", 0);
                LSG_SetListItems("List", newItems);

                auto items     = LSG_GetListItems("List");
                auto pageItems = LSG_GetPageListItems("List");

                Assert::AreEqual(200,                   (int)items.size());
                Assert::AreEqual(LSG_MAX_ROWS_PER_PAGE, (int)pageItems.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(Sort)
        {
            try
            {
                SetItems();

                LSG_SortList("List", LSG_SORT_ORDER_DESCENDING);

                auto item0   = LSG_GetListItem("List", 0);
                auto item199 = LSG_GetListItem("List", 199);

                Assert::AreEqual("My new list item number 99", item0.c_str());
                Assert::AreEqual("My new list item number 0",  item199.c_str());

                LSG_SortList("List", LSG_SORT_ORDER_ASCENDING);

                item0   = LSG_GetListItem("List", 0);
                item199 = LSG_GetListItem("List", 199);

                Assert::AreEqual("My new list item number 0",  item0.c_str());
                Assert::AreEqual("My new list item number 99", item199.c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
	};

	TEST_CLASS(TableHeader)
	{
        TEST_METHOD(GetHeader)
        {
            try
            {
                auto header = LSG_GetTableHeader("Table");

                Assert::AreEqual(2, (int)header.size());

                Assert::AreEqual("DOLOR", header[0].c_str());
                Assert::AreEqual("MAGNA", header[1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(RemoveHeader)
        {
            try
            {
                LSG_RemoveTableHeader("Table");

                auto header = LSG_GetTableHeader("Table");

                Assert::IsTrue(header.empty());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetHeader)
        {
            try
            {
                LSG_SetTableHeader("Table", { "New Header A", "New Header B", "New Header C" });

                auto header = LSG_GetTableHeader("Table");

                Assert::AreEqual(3, (int)header.size());

                Assert::AreEqual("New Header A", header[0].c_str());
                Assert::AreEqual("New Header B", header[1].c_str());
                Assert::AreEqual("New Header C", header[2].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
    };

    TEST_CLASS(TableGroup)
    {
        TEST_METHOD(AddGroup)
        {
            try
            {
                for (int i = 0; i < 20; i++)
                {
                    LSG_TableGroup newGroup = { .group = ("My new group " + std::to_string(i)) };

                    for (int j = 0; j < 10; j++)
                        newGroup.rows.push_back({ ("My new group row " + std::to_string(j) + " - Column A"), ("My new group row " + std::to_string(j) + " - Column B") });

                    LSG_AddTableGroup("TableWithGroups", newGroup);
                }

                auto groups     = LSG_GetTableGroups("TableWithGroups");
                auto pageGroups = LSG_GetPageTableGroups("TableWithGroups");

                Assert::AreEqual(22, (int)groups.size());
                Assert::AreEqual(10, (int)pageGroups.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetGroup)
        {
            try
            {
                SetGroups();

                auto groupN1 = LSG_GetTableGroup("TableWithGroups", "My new group -1");
                auto group0  = LSG_GetTableGroup("TableWithGroups", "My new group 0");
                auto group19 = LSG_GetTableGroup("TableWithGroups", "My new group 19");

                Assert::IsTrue(groupN1.group.empty());
                Assert::IsTrue(groupN1.rows.empty());

                Assert::AreEqual(10, (int)group0.rows.size());
                Assert::AreEqual(10, (int)group19.rows.size());

                Assert::AreEqual("My new group 0",                group0.group.c_str());
                Assert::AreEqual("My new group row 0 - Column A", group0.rows[0][0].c_str());
                Assert::AreEqual("My new group row 0 - Column B", group0.rows[0][1].c_str());

                Assert::AreEqual("My new group 19",               group19.group.c_str());
                Assert::AreEqual("My new group row 9 - Column A", group19.rows[9][0].c_str());
                Assert::AreEqual("My new group row 9 - Column B", group19.rows[9][1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetGroups)
        {
            try
            {
                SetGroups();

                auto groups     = LSG_GetTableGroups("TableWithGroups");
                auto pageGroups = LSG_GetPageTableGroups("TableWithGroups");

                Assert::AreEqual(20, (int)groups.size());
                Assert::AreEqual(9,  (int)pageGroups.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(RemoveGroup)
        {
            try
            {
                SetGroups();

                LSG_RemoveTableGroup("TableWithGroups", "My new group -1");
                LSG_RemoveTableGroup("TableWithGroups", "My new group 0");
                LSG_RemoveTableGroup("TableWithGroups", "My new group 19");

                auto group0  = LSG_GetTableGroup("TableWithGroups", "My new group 0");
                auto group17 = LSG_GetTableGroup("TableWithGroups", "My new group 17");
                auto group18 = LSG_GetTableGroup("TableWithGroups", "My new group 18");
                auto group19 = LSG_GetTableGroup("TableWithGroups", "My new group 19");

                Assert::IsTrue(group0.group.empty());
                Assert::IsTrue(group0.rows.empty());

                Assert::IsTrue(group19.group.empty());
                Assert::IsTrue(group19.rows.empty());

                Assert::AreEqual(10, (int)group17.rows.size());
                Assert::AreEqual(10, (int)group18.rows.size());

                Assert::AreEqual("My new group 17",               group17.group.c_str());
                Assert::AreEqual("My new group row 0 - Column A", group17.rows[0][0].c_str());
                Assert::AreEqual("My new group row 0 - Column B", group17.rows[0][1].c_str());

                Assert::AreEqual("My new group 18",               group18.group.c_str());
                Assert::AreEqual("My new group row 9 - Column A", group18.rows[9][0].c_str());
                Assert::AreEqual("My new group row 9 - Column B", group18.rows[9][1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetGroup)
        {
            try
            {
                SetGroups();

                LSG_SetTableGroup("TableWithGroups", { .group = "My new group -1", .rows = {{ ("My non-existing group row - Column A"), ("My non-existing group row - Column B") }} });
                LSG_SetTableGroup("TableWithGroups", { .group = "My new group 0",  .rows = {{ ("My first group row - Column A"),        ("My first group row - Column B") }} });
                LSG_SetTableGroup("TableWithGroups", { .group = "My new group 19", .rows = {{ ("My last group row - Column A"),         ("My last group row - Column B") }} });

                auto groupN1 = LSG_GetTableGroup("TableWithGroups", "My new group -1");
                auto group0  = LSG_GetTableGroup("TableWithGroups", "My new group 0");
                auto group19 = LSG_GetTableGroup("TableWithGroups", "My new group 19");

                Assert::IsTrue(groupN1.group.empty());
                Assert::IsTrue(groupN1.rows.empty());

                Assert::AreEqual(1, (int)group0.rows.size());
                Assert::AreEqual(1, (int)group19.rows.size());

                Assert::AreEqual("My new group 0",                group0.group.c_str());
                Assert::AreEqual("My first group row - Column A", group0.rows[0][0].c_str());
                Assert::AreEqual("My first group row - Column B", group0.rows[0][1].c_str());

                Assert::AreEqual("My new group 19",              group19.group.c_str());
                Assert::AreEqual("My last group row - Column A", group19.rows[0][0].c_str());
                Assert::AreEqual("My last group row - Column B", group19.rows[0][1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetGroups)
        {
            try
            {
                LSG_TableGroups newGroups = {};

                for (int i = 0; i < 20; i++)
                {
                    LSG_TableGroup newGroup = { .group = ("My new group " + std::to_string(i)) };

                    for (int j = 0; j < 10; j++)
                        newGroup.rows.push_back({ ("My new group row " + std::to_string(j) + " - Column A"), ("My new group row " + std::to_string(j) + " - Column B") });

                    newGroups.push_back(newGroup);
                }

                LSG_SetPage("TableWithGroups", 0);
                LSG_SetTableGroups("TableWithGroups", newGroups);

                auto groups     = LSG_GetTableGroups("TableWithGroups");
                auto pageGroups = LSG_GetPageTableGroups("TableWithGroups");

                Assert::AreEqual(20, (int)groups.size());
                Assert::AreEqual(9,  (int)pageGroups.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(Sort)
        {
            try
            {
                SetGroups();

                LSG_SortTable("TableWithGroups", LSG_SORT_ORDER_DESCENDING, 1);

                auto group0 = LSG_GetTableGroup("TableWithGroups", "My new group 0");

                Assert::AreEqual(10, (int)group0.rows.size());

                Assert::AreEqual("My new group 0",                group0.group.c_str());
                Assert::AreEqual("My new group row 9 - Column A", group0.rows[0][0].c_str());
                Assert::AreEqual("My new group row 9 - Column B", group0.rows[0][1].c_str());
                Assert::AreEqual("My new group row 0 - Column A", group0.rows[9][0].c_str());
                Assert::AreEqual("My new group row 0 - Column B", group0.rows[9][1].c_str());

                LSG_SortTable("TableWithGroups", LSG_SORT_ORDER_ASCENDING, 1);

                group0 = LSG_GetTableGroup("TableWithGroups", "My new group 0");

                Assert::AreEqual(10, (int)group0.rows.size());

                Assert::AreEqual("My new group 0",                group0.group.c_str());
                Assert::AreEqual("My new group row 0 - Column A", group0.rows[0][0].c_str());
                Assert::AreEqual("My new group row 0 - Column B", group0.rows[0][1].c_str());
                Assert::AreEqual("My new group row 9 - Column A", group0.rows[9][0].c_str());
                Assert::AreEqual("My new group row 9 - Column B", group0.rows[9][1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
    };

    TEST_CLASS(TableRow)
	{
        TEST_METHOD(AddRow)
        {
            try
            {
                for (int i = 0; i < 200; i++)
                    LSG_AddTableRow("Table", { ("My new table row " + std::to_string(i) + " - Column A"), ("My new table row " + std::to_string(i) + " - Column B") });

                auto rows     = LSG_GetTableRows("Table");
                auto pageRows = LSG_GetPageTableRows("Table");

                Assert::AreEqual(206,                   (int)rows.size());
                Assert::AreEqual(LSG_MAX_ROWS_PER_PAGE, (int)pageRows.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetRow)
        {
            try
            {
                SetRows();

                auto rowN1  = LSG_GetTableRow("Table", -1);
                auto row0   = LSG_GetTableRow("Table", 0);
                auto row199 = LSG_GetTableRow("Table", 199);

                Assert::IsTrue(rowN1.empty());

                Assert::AreEqual(2, (int)row0.size());
                Assert::AreEqual(2, (int)row199.size());

                Assert::AreEqual("My new table row 0 - Column A", row0[0].c_str());
                Assert::AreEqual("My new table row 0 - Column B", row0[1].c_str());

                Assert::AreEqual("My new table row 99 - Column A", row199[0].c_str());
                Assert::AreEqual("My new table row 99 - Column B", row199[1].c_str());

                auto pageRowN1  = LSG_GetPageTableRow("Table", -1);
                auto pageRow0   = LSG_GetPageTableRow("Table", 0);
                auto pageRow199 = LSG_GetPageTableRow("Table", 199);

                Assert::IsTrue(pageRowN1.empty());
                Assert::IsTrue(pageRow199.empty());

                Assert::AreEqual(2, (int)pageRow0.size());

                Assert::AreEqual("My new table row 0 - Column A", pageRow0[0].c_str());
                Assert::AreEqual("My new table row 0 - Column B", pageRow0[1].c_str());

                LSG_SetPage("Table", 1);

                row0   = LSG_GetTableRow("Table", 0);
                row199 = LSG_GetTableRow("Table", 199);

                Assert::AreEqual(2, (int)row0.size());
                Assert::AreEqual(2, (int)row199.size());

                Assert::AreEqual("My new table row 0 - Column A", row0[0].c_str());
                Assert::AreEqual("My new table row 0 - Column B", row0[1].c_str());

                Assert::AreEqual("My new table row 99 - Column A", row199[0].c_str());
                Assert::AreEqual("My new table row 99 - Column B", row199[1].c_str());

                pageRow0   = LSG_GetPageTableRow("Table", 0);
                pageRow199 = LSG_GetPageTableRow("Table", 199);

                Assert::IsTrue(pageRow199.empty());

                Assert::AreEqual(2, (int)pageRow0.size());

                Assert::AreEqual("My new table row 189 - Column A", pageRow0[0].c_str());
                Assert::AreEqual("My new table row 189 - Column B", pageRow0[1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetRows)
        {
            try
            {
                SetRows();

                auto rows     = LSG_GetTableRows("Table");
                auto pageRows = LSG_GetPageTableRows("Table");

                Assert::AreEqual(200,                   (int)rows.size());
                Assert::AreEqual(LSG_MAX_ROWS_PER_PAGE, (int)pageRows.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(RemoveRow)
        {
            try
            {
                SetRows();

                LSG_RemoveTableRow("Table", -1);
                LSG_RemoveTableRow("Table", 0);
                LSG_RemoveTableRow("Table", 199);

                auto row0   = LSG_GetTableRow("Table", 0);
                auto row197 = LSG_GetTableRow("Table", 197);
                auto row198 = LSG_GetTableRow("Table", 198);
                auto row199 = LSG_GetTableRow("Table", 199);

                Assert::IsTrue(row199.empty());

                Assert::AreEqual(2, (int)row0.size());
                Assert::AreEqual(2, (int)row197.size());
                Assert::AreEqual(2, (int)row198.size());

                Assert::AreEqual("My new table row 1 - Column A", row0[0].c_str());
                Assert::AreEqual("My new table row 1 - Column B", row0[1].c_str());

                Assert::AreEqual("My new table row 98 - Column A", row197[0].c_str());
                Assert::AreEqual("My new table row 98 - Column B", row197[1].c_str());

                Assert::AreEqual("My new table row 99 - Column A", row198[0].c_str());
                Assert::AreEqual("My new table row 99 - Column B", row198[1].c_str());

                LSG_RemovePageTableRow("Table", -1);
                LSG_RemovePageTableRow("Table", 0);
                LSG_RemovePageTableRow("Table", 198);

                auto pageRow0 = LSG_GetPageTableRow("Table", 0);

                Assert::AreEqual(2, (int)pageRow0.size());

                Assert::AreEqual("My new table row 10 - Column A", pageRow0[0].c_str());
                Assert::AreEqual("My new table row 10 - Column B", pageRow0[1].c_str());

                LSG_SetPage("Table", 1);

                row0 = LSG_GetTableRow("Table", 0);

                Assert::AreEqual(2, (int)row0.size());

                Assert::AreEqual("My new table row 10 - Column A", row0[0].c_str());
                Assert::AreEqual("My new table row 10 - Column B", row0[1].c_str());

                pageRow0 = LSG_GetPageTableRow("Table", 0);

                Assert::AreEqual(2, (int)pageRow0.size());

                Assert::AreEqual("My new table row 190 - Column A", pageRow0[0].c_str());
                Assert::AreEqual("My new table row 190 - Column B", pageRow0[1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetRow)
        {
            try
            {
                SetRows();

                LSG_SetTableRow("Table", 0, { ("A - My first table row - Column A"), ("A - My first table row - Column B") });
                LSG_SetTableRow("Table", 199, { ("Z - My last table row - Column A"), ("Z - My last table row - Column B") });

                auto row0   = LSG_GetTableRow("Table", 0);
                auto row199 = LSG_GetTableRow("Table", 199);

                Assert::AreEqual(2, (int)row0.size());
                Assert::AreEqual(2, (int)row199.size());

                Assert::AreEqual("A - My first table row - Column A", row0[0].c_str());
                Assert::AreEqual("A - My first table row - Column B", row0[1].c_str());

                Assert::AreEqual("Z - My last table row - Column A", row199[0].c_str());
                Assert::AreEqual("Z - My last table row - Column B", row199[1].c_str());

                LSG_SetPageTableRow("Table", 0, { ("A - My first page table row - Column A"), ("A - My first page table row - Column B") });
                LSG_SetPageTableRow("Table", 199, { ("Z - My last page table row - Column A"), ("Z - My last page table row - Column B") });

                auto pageRow0   = LSG_GetPageTableRow("Table", 0);
                auto pageRow199 = LSG_GetPageTableRow("Table", 199);

                Assert::IsTrue(pageRow199.empty());

                Assert::AreEqual(2, (int)pageRow0.size());

                Assert::AreEqual("A - My first page table row - Column A", pageRow0[0].c_str());
                Assert::AreEqual("A - My first page table row - Column B", pageRow0[1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetRows)
        {
            try
            {
                LSG_TableRows newRows = {};

                for (int i = 0; i < 200; i++)
                    newRows.push_back({ ("My new table row " + std::to_string(i) + " - Column A"), ("My new table row " + std::to_string(i) + " - Column B") });

                LSG_SetPage("Table", 0);
                LSG_SetTableRows("Table", newRows);

                auto rows     = LSG_GetTableRows("Table");
                auto pageRows = LSG_GetPageTableRows("Table");

                Assert::AreEqual(200,                   (int)rows.size());
                Assert::AreEqual(LSG_MAX_ROWS_PER_PAGE, (int)pageRows.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(Sort)
        {
            try
            {
                SetRows();

                LSG_SortTable("Table", LSG_SORT_ORDER_DESCENDING, 1);

                auto row0   = LSG_GetTableRow("Table", 0);
                auto row199 = LSG_GetTableRow("Table", 199);

                Assert::AreEqual(2, (int)row0.size());
                Assert::AreEqual(2, (int)row199.size());

                Assert::AreEqual("My new table row 99 - Column A", row0[0].c_str());
                Assert::AreEqual("My new table row 99 - Column B", row0[1].c_str());

                Assert::AreEqual("My new table row 0 - Column A", row199[0].c_str());
                Assert::AreEqual("My new table row 0 - Column B", row199[1].c_str());

                LSG_SortTable("Table", LSG_SORT_ORDER_ASCENDING, 1);

                row0   = LSG_GetTableRow("Table", 0);
                row199 = LSG_GetTableRow("Table", 199);

                Assert::AreEqual(2, (int)row0.size());
                Assert::AreEqual(2, (int)row199.size());

                Assert::AreEqual("My new table row 0 - Column A", row0[0].c_str());
                Assert::AreEqual("My new table row 0 - Column B", row0[1].c_str());

                Assert::AreEqual("My new table row 99 - Column A", row199[0].c_str());
                Assert::AreEqual("My new table row 99 - Column B", row199[1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
    };

    TEST_CLASS(Text)
    {
        TEST_METHOD(SetText)
        {
            try
            {
                LSG_SetText("TextWrap", "New test value\nwith wrap.");

                auto text = LSG_GetText("TextWrap");

                Assert::AreEqual("New test value\nwith wrap.", text.c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
    };

    TEST_CLASS(TextInput)
    {
        TEST_METHOD(Clear)
        {
            try
            {
                SetValue();

                auto value1 = LSG_GetTextInputValue("TextInput");

                Assert::IsFalse(value1.empty());

                LSG_ClearTextInput("TextInput");

                auto value2 = LSG_GetTextInputValue("TextInput");

                Assert::IsTrue(value2.empty());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetValue)
        {
            try
            {
                LSG_SetTextInputValue("TextInput", "New test value");

                auto value = LSG_GetTextInputValue("TextInput");

                Assert::AreEqual("New test value", value.c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
    };

}

#endif
